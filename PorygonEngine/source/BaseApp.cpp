#include "BaseApp.h"
#include "ResourceManager.h"

int BaseApp::run(HINSTANCE hInst, int nCmdShow)
{
    if (FAILED(m_window.init(hInst, nCmdShow, WndProc))) {
        return 0;
    }

    if (FAILED(init()))
        return 0;

    MSG msg = {};
    LARGE_INTEGER freq, prev;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&prev);

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            LARGE_INTEGER curr;
            QueryPerformanceCounter(&curr);
            float deltaTime = static_cast<float>(curr.QuadPart - prev.QuadPart) / freq.QuadPart;
            prev = curr;

            update(deltaTime);
            render();
        }
    }

    return (int)msg.wParam;
}

HRESULT BaseApp::init()
{
    HRESULT hr = S_OK;

    hr = m_swapChain.init(m_device, m_deviceContext, m_backBuffer, m_window);
    if (FAILED(hr)) return hr;

    hr = m_renderTargetView.init(m_device, m_backBuffer, DXGI_FORMAT_R8G8B8A8_UNORM);
    if (FAILED(hr)) return hr;

    hr = m_depthStencil.init(
        m_device,
        m_window.m_width,
        m_window.m_height,
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        D3D11_BIND_DEPTH_STENCIL,
        4, 0
    );
    if (FAILED(hr)) return hr;

    hr = m_depthStencilView.init(m_device, m_depthStencil, DXGI_FORMAT_D24_UNORM_S8_UINT);
    if (FAILED(hr)) return hr;

    hr = m_viewport.init(m_window);
    if (FAILED(hr)) return hr;

    // -------------------------------
    // Load CyberGun Actor
    // -------------------------------
    m_cyberGun = EU::MakeShared<Actor>(m_device);

    if (!m_cyberGun.isNull())
    {
        m_model = new Model3D("Assets/cybergun.fbx", ModelType::FBX);

        std::vector<MeshComponent> cyberGunMeshes = m_model->GetMeshes();
        std::vector<Texture> cyberGunTextures;

        hr = m_cyberGunAlbedo.init(m_device, "Assets/UltraTTexture", ExtensionType::JPG);
        if (FAILED(hr)) return hr;

        cyberGunTextures.push_back(m_cyberGunAlbedo);

        m_cyberGun->setMesh(m_device, cyberGunMeshes);
        m_cyberGun->setTextures(cyberGunTextures);
        m_cyberGun->setName("cybergun");

        m_actors.push_back(m_cyberGun);

        // TRANSFORMACIÓN (PERFIL)
        m_cyberGun->getComponent<Transform>()->setTransform(
            EU::Vector3(2.0f, -4.90f, 11.60f),           // Translation
            EU::Vector3(-XM_PIDIV2, -XM_PIDIV2, 0.0f),   // Rotation (Perfil)
            EU::Vector3(0.01f, 0.01f, 0.01f)             // Scale
        );
    }
    else {
        return E_FAIL;
    }

    // INPUT LAYOUT -------------------

    std::vector<D3D11_INPUT_ELEMENT_DESC> Layout;

    D3D11_INPUT_ELEMENT_DESC position = {};
    position.SemanticName = "POSITION";
    position.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    position.InputSlot = 0;
    position.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    position.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    Layout.push_back(position);

    D3D11_INPUT_ELEMENT_DESC texcoord = {};
    texcoord.SemanticName = "TEXCOORD";
    texcoord.Format = DXGI_FORMAT_R32G32_FLOAT;
    texcoord.InputSlot = 0;
    texcoord.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    texcoord.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    Layout.push_back(texcoord);

    hr = m_shaderProgram.init(m_device, "PorygonEngine.fx", Layout);
    if (FAILED(hr)) return hr;

    hr = m_cbNeverChanges.init(m_device, sizeof(CBNeverChanges));
    if (FAILED(hr)) return hr;

    hr = m_cbChangeOnResize.init(m_device, sizeof(CBChangeOnResize));
    if (FAILED(hr)) return hr;

    // ------------------------------------------------
    // CÁMARA (SIN CAMBIOS - CERCA/ZOOM IN)
    // ------------------------------------------------

    // Apuntamos al centro del arma
    XMVECTOR At = XMVectorSet(2.0f, -4.90f, 11.60f, 0.0f);

    // Mantenemos la posición 10.80f como pediste (muy cerca)
    XMVECTOR Eye = XMVectorSet(2.0f, -4.90f, 10.80f, 0.0f);

    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    m_View = XMMatrixLookAtLH(Eye, At, Up);
    cbNeverChanges.mView = XMMatrixTranspose(m_View);

    m_Projection = XMMatrixPerspectiveFovLH(
        XM_PIDIV4,
        m_window.m_width / (FLOAT)m_window.m_height,
        0.01f,
        100.0f
    );

    cbChangesOnResize.mProjection = XMMatrixTranspose(m_Projection);

    return S_OK;
}

void BaseApp::update(float deltaTime)
{
    // Actualizamos los constant buffers de vista/proyección
    cbNeverChanges.mView = XMMatrixTranspose(m_View);
    m_cbNeverChanges.update(m_deviceContext, nullptr, 0, nullptr, &cbNeverChanges, 0, 0);

    m_Projection = XMMatrixPerspectiveFovLH(
        XM_PIDIV4,
        m_window.m_width / (FLOAT)m_window.m_height,
        0.01f,
        100.0f
    );

    cbChangesOnResize.mProjection = XMMatrixTranspose(m_Projection);
    m_cbChangeOnResize.update(m_deviceContext, nullptr, 0, nullptr, &cbChangesOnResize, 0, 0);

    // Actualizar actores
    for (auto& actor : m_actors)
        actor->update(deltaTime, m_deviceContext);
}

void BaseApp::render()
{
    float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

    m_renderTargetView.render(m_deviceContext, m_depthStencilView, 1, ClearColor);
    m_viewport.render(m_deviceContext);
    m_depthStencilView.render(m_deviceContext);
    m_shaderProgram.render(m_deviceContext);

    m_cbNeverChanges.render(m_deviceContext, 0, 1);
    m_cbChangeOnResize.render(m_deviceContext, 1, 1);

    for (auto& actor : m_actors)
        actor->render(m_deviceContext);

    m_swapChain.present();
}

void BaseApp::destroy()
{
    // ---------------------------------------------------------
    // LIMPIEZA DE MEMORIA AGREGADA (Solución de logs)
    // ---------------------------------------------------------

    // 1. Limpiar lista de actores
    m_actors.clear();

    // 2. Liberar textura
    m_cyberGunAlbedo.destroy();

    // 3. Eliminar puntero del modelo cargado con 'new'
    if (m_model)
    {
        delete m_model;
        m_model = nullptr;
    }

    // ---------------------------------------------------------
    // LIMPIEZA ESTÁNDAR
    // ---------------------------------------------------------
    if (m_deviceContext.m_deviceContext)
        m_deviceContext.m_deviceContext->ClearState();

    m_cbNeverChanges.destroy();
    m_cbChangeOnResize.destroy();
    m_shaderProgram.destroy();
    m_depthStencil.destroy();
    m_depthStencilView.destroy();
    m_renderTargetView.destroy();
    m_swapChain.destroy();
    m_backBuffer.destroy();
    m_deviceContext.destroy();
    m_device.destroy();
}

LRESULT BaseApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}