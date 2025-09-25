// SwapChain.cpp
#include "../include/Prerequisites.h"
#include "../include/SwapChain.h"
#include "../include/Device.h"
#include "../include/DeviceContext.h"
#include "../include/Window.h"
#include "../include/Texture.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) do { if (p) { (p)->Release(); (p) = nullptr; } } while(0)
#endif
#ifndef ERROR
#define ERROR(MOD, FUNC, WMSG) OutputDebugStringW(L"[ERROR][" L#MOD L"][" L#FUNC L"] " WMSG L"\n")
#endif
#ifndef MESSAGE
#define MESSAGE(MOD, FUNC, WMSG) OutputDebugStringW(L"[INFO ][" L#MOD L"][" L#FUNC L"] " WMSG L"\n")
#endif

void SwapChain::destroyRTV_() { SAFE_RELEASE(m_rtv); }

void SwapChain::destroy()
{
    // Si estuviera en fullscreen, vuelve a windowed antes de liberar
    if (m_swap) { m_swap->SetFullscreenState(FALSE, nullptr); }

    // RTV primero, luego swap chain
    destroyRTV_();
    SAFE_RELEASE(m_swap);

    m_width = m_height = 0;
    m_format = DXGI_FORMAT_R8G8B8A8_UNORM;
    MESSAGE(SwapChain, destroy, L"Released");
}

// ============================================================================
// INIT: Device + Immediate Context + Factory->CreateSwapChain + RTV + Viewport
// ============================================================================
HRESULT SwapChain::init(Device& device,
    DeviceContext& deviceContext,
    Texture& backBuffer,
    Window window)
{
    if (!window.m_hWnd) { ERROR(SwapChain, init, L"Invalid window handle"); return E_POINTER; }

    // 1) Crear Device + Immediate Context (sin swap chain)
    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] = {
      D3D_DRIVER_TYPE_HARDWARE,
      D3D_DRIVER_TYPE_WARP,
      D3D_DRIVER_TYPE_REFERENCE
    };
    const UINT numDriverTypes = _countof(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0
    };
    const UINT numFeatureLevels = _countof(featureLevels);

    ID3D11Device* d3dDevice = nullptr;
    ID3D11DeviceContext* immContext = nullptr;
    D3D_FEATURE_LEVEL    obtainedFL = {};
    HRESULT hr = E_FAIL;

    for (UINT i = 0; i < numDriverTypes; ++i) {
        hr = D3D11CreateDevice(
            nullptr, driverTypes[i], nullptr, createDeviceFlags,
            featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &d3dDevice, &obtainedFL, &immContext
        );
        if (SUCCEEDED(hr)) break;
    }
    if (FAILED(hr)) { ERROR(SwapChain, init, L"D3D11CreateDevice failed"); return hr; }

    // Conectar wrappers
    device.m_device = d3dDevice;          // si tienes attach(), �salo
    deviceContext.attach(immContext);     // tu wrapper hace AddRef/Release

    // 2) Obtener Factory v�a IDXGIDevice -> IDXGIAdapter -> IDXGIFactory
    IDXGIDevice* dxgiDev = nullptr;
    IDXGIAdapter* adapter = nullptr;
    IDXGIFactory* factory = nullptr;

    hr = d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDev);
    if (FAILED(hr)) { ERROR(SwapChain, init, L"QueryInterface IDXGIDevice failed"); goto FactoryCleanup; }

    hr = dxgiDev->GetAdapter(&adapter);
    if (FAILED(hr)) { ERROR(SwapChain, init, L"IDXGIDevice::GetAdapter failed"); goto FactoryCleanup; }

    hr = adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(hr)) { ERROR(SwapChain, init, L"IDXGIAdapter::GetParent IDXGIFactory failed"); goto FactoryCleanup; }

    // 3) Crear Swap Chain (DXGI 1.0, backbuffer sin MSAA)
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferDesc.Width = 0; // usar tama�o de ventana
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.SampleDesc.Count = 1; // backbuffer sin MSAA (compat max)
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;       // cambia a 2 si quieres doble buffer
    sd.OutputWindow = window.m_hWnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    SAFE_RELEASE(m_swap); // por si acaso
    hr = factory->CreateSwapChain(d3dDevice, &sd, &m_swap);
    if (FAILED(hr)) { ERROR(SwapChain, init, L"CreateSwapChain failed"); goto FactoryCleanup; }

    // 4) RTV del backbuffer y exponer el recurso en backBuffer
    ID3D11Texture2D* bbTex = nullptr;
    hr = m_swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&bbTex);
    if (FAILED(hr)) { ERROR(SwapChain, init, L"GetBuffer(0) failed"); goto FactoryCleanup; }

    SAFE_RELEASE(m_rtv);
    hr = d3dDevice->CreateRenderTargetView(bbTex, nullptr, &m_rtv);
    if (FAILED(hr)) { SAFE_RELEASE(bbTex); ERROR(SwapChain, init, L"CreateRenderTargetView failed"); goto FactoryCleanup; }

    backBuffer.destroy();
    backBuffer.m_texture = bbTex; // nos quedamos con la ref (no crear SRV del backbuffer)

    // 5) Guardar tama�o/formato y configurar viewport inicial
    DXGI_SWAP_CHAIN_DESC got{};
    m_swap->GetDesc(&got);
    m_width = got.BufferDesc.Width;
    m_height = got.BufferDesc.Height;
    m_format = got.BufferDesc.Format;

    D3D11_VIEWPORT vp{};
    vp.TopLeftX = 0.0f; vp.TopLeftY = 0.0f;
    vp.Width = static_cast<FLOAT>(m_width ? m_width : 1);
    vp.Height = static_cast<FLOAT>(m_height ? m_height : 1);
    vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
    deviceContext.RSSetViewports(1, &vp);

    MESSAGE(SwapChain, init, L"Device+Context+Factory+SwapChain initialized");

FactoryCleanup:
    SAFE_RELEASE(factory);
    SAFE_RELEASE(adapter);
    SAFE_RELEASE(dxgiDev);
    return hr;
}

// ============================================================================
// LOW-LEVEL API
// ============================================================================
HRESULT SwapChain::create(ID3D11Device* device,
    HWND hwnd,
    UINT width,
    UINT height,
    DXGI_FORMAT format,
    UINT bufferCount,
    BOOL windowed,
    UINT /*sampleCount*/)
{
    if (!device) { ERROR(SwapChain, create, L"device is nullptr"); return E_POINTER; }
    if (!hwnd) { ERROR(SwapChain, create, L"hwnd is nullptr");   return E_POINTER; }
    if (width == 0 || height == 0) { ERROR(SwapChain, create, L"invalid size"); return E_INVALIDARG; }

    destroy();

    m_width = width; m_height = height; m_format = format;

    IDXGIDevice* dxgiDev = nullptr;
    IDXGIAdapter* adapter = nullptr;
    IDXGIFactory* factory = nullptr;

    HRESULT hr = device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDev);
    if (FAILED(hr)) { ERROR(SwapChain, create, L"QIx IDXGIDevice failed"); goto Cleanup; }

    hr = dxgiDev->GetAdapter(&adapter);
    if (FAILED(hr)) { ERROR(SwapChain, create, L"GetAdapter failed"); goto Cleanup; }

    hr = adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(hr)) { ERROR(SwapChain, create, L"GetParent IDXGIFactory failed"); goto Cleanup; }

    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = format;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = bufferCount;
    sd.OutputWindow = hwnd;
    sd.Windowed = windowed;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    hr = factory->CreateSwapChain(device, &sd, &m_swap);
    if (FAILED(hr)) { ERROR(SwapChain, create, L"CreateSwapChain failed"); goto Cleanup; }

    hr = recreateRTV(device);
    if (FAILED(hr)) goto Cleanup;

    MESSAGE(SwapChain, create, L"Created");

Cleanup:
    SAFE_RELEASE(factory);
    SAFE_RELEASE(adapter);
    SAFE_RELEASE(dxgiDev);
    return hr;
}

HRESULT SwapChain::recreateRTV(ID3D11Device* device)
{
    if (!m_swap) { ERROR(SwapChain, recreateRTV, L"swap chain is nullptr"); return E_FAIL; }
    if (!device) { ERROR(SwapChain, recreateRTV, L"device is nullptr");     return E_POINTER; }

    destroyRTV_();

    ID3D11Texture2D* backBuffer = nullptr;
    HRESULT hr = m_swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(hr)) { ERROR(SwapChain, recreateRTV, L"GetBuffer(0) failed"); return hr; }

    hr = device->CreateRenderTargetView(backBuffer, nullptr, &m_rtv);
    SAFE_RELEASE(backBuffer);

    if (FAILED(hr)) { ERROR(SwapChain, recreateRTV, L"CreateRenderTargetView failed"); return hr; }

    MESSAGE(SwapChain, recreateRTV, L"RTV_Recreated");
    return S_OK;
}

HRESULT SwapChain::resize(ID3D11Device* device, UINT width, UINT height)
{
    if (!m_swap) { ERROR(SwapChain, resize, L"swap chain is nullptr"); return E_FAIL; }
    if (!device) { ERROR(SwapChain, resize, L"device is nullptr");     return E_POINTER; }
    if (width == 0 || height == 0) { ERROR(SwapChain, resize, L"invalid size"); return E_INVALIDARG; }

    destroyRTV_();

    HRESULT hr = m_swap->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) { ERROR(SwapChain, resize, L"ResizeBuffers failed"); return hr; }

    m_width = width; m_height = height;

    hr = recreateRTV(device);
    if (FAILED(hr)) return hr;

    // Nota: re-aplica viewport desde el caller usando DeviceContext::RSSetViewports.
    MESSAGE(SwapChain, resize, L"Resized");
    return S_OK;
}

HRESULT SwapChain::present(UINT syncInterval, UINT flags)
{
    if (!m_swap) { ERROR(SwapChain, present, L"swap chain is nullptr"); return E_FAIL; }

    HRESULT hr = m_swap->Present(syncInterval, flags);

    if (hr == DXGI_STATUS_OCCLUDED) {
        // ventana oculta: no es fatal
        return hr;
    }
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
        ERROR(SwapChain, present, L"Device removed/reset during Present()");
        return hr;
    }
    if (FAILED(hr)) {
        ERROR(SwapChain, present, L"Present failed");
        return hr;
    }
    return S_OK;
}

void SwapChain::bindAsRenderTarget(ID3D11DeviceContext* ctx,
    ID3D11DepthStencilView* dsv) const
{
    if (!ctx) { ERROR(SwapChain, bindAsRenderTarget, L"ctx is nullptr"); return; }
    if (!m_rtv) { ERROR(SwapChain, bindAsRenderTarget, L"RTV is nullptr"); return; }
    ID3D11RenderTargetView* rtv = m_rtv;
    ctx->OMSetRenderTargets(1, &rtv, dsv);
}

// ============================================================================
// WRAPPERS
// ============================================================================
HRESULT SwapChain::create(Device& device,
    Window& window,
    UINT width,
    UINT height,
    DXGI_FORMAT format,
    UINT bufferCount,
    BOOL windowed,
    UINT sampleCount)
{
    return create(device.m_device, window.m_hWnd, width, height,
        format, bufferCount, windowed, sampleCount);
}
HRESULT SwapChain::recreateRTV(Device& device) { return recreateRTV(device.m_device); }
HRESULT SwapChain::resize(Device& device, UINT width, UINT height) { return resize(device.m_device, width, height); }
void SwapChain::bindAsRenderTarget(DeviceContext& ctx, ID3D11DepthStencilView* dsv) const
{
    if (!m_rtv) { ERROR(SwapChain, bindAsRenderTarget, L"RTV is nullptr"); return; }
    ID3D11RenderTargetView* rtv = m_rtv;
    ctx.OMSetRenderTargets(1, &rtv, dsv);
}