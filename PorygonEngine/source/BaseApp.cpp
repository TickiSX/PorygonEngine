#include "BaseApp.h"
#include "ResourceManager.h"

HRESULT BaseApp::awake() {
    HRESULT hr = S_OK;
    m_sceneGraph.init();
    MESSAGE("Main", "Awake", "Application awake successfully.");
    return hr;
}

int BaseApp::run(HINSTANCE hInst, int nCmdShow) {
    if (FAILED(m_window.init(hInst, nCmdShow, WndProc, this))) {
        ERROR("Main", "Run", "Failed to initialize window.");
        return 0;
    }
    if (FAILED(awake())) {
        ERROR("Main", "Run", "Failed to awake application.");
        return 0;
    }
    if (FAILED(init())) {
        // Cambiado para que no sea un retorno fatal inmediato si prefieres debugear la GUI
        ERROR("Main", "Run", "Failed to initialize device and device context.");
        return 0;
    }

    m_gui.init(m_window, m_device, m_deviceContext);

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

    // IMPORTANTE: Llamar a destroy al salir del loop
    destroy();
    return (int)msg.wParam;
}

HRESULT BaseApp::init() {
    HRESULT hr = S_OK;

    // --- RUTA RAIZ ABSOLUTA apuntando a tu carpeta bin/ ---
    const std::string ROOT_PATH = "C:/6TOCuatri/PorygonEngine/PorygonEngine/bin/";

    // --- Infraestructura D3D11 ---
    hr = m_swapChain.init(m_device, m_deviceContext, m_backBuffer, m_window);
    if (FAILED(hr)) return hr;

    hr = m_renderTargetView.init(m_device, m_backBuffer, DXGI_FORMAT_R8G8B8A8_UNORM);
    if (FAILED(hr)) return hr;

    hr = m_depthStencil.init(m_device, m_window.m_width, m_window.m_height,
        DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, 4, 0);
    if (FAILED(hr)) return hr;

    hr = m_depthStencilView.init(m_device, m_depthStencil, DXGI_FORMAT_D24_UNORM_S8_UINT);
    if (FAILED(hr)) return hr;

    hr = m_viewport.init(m_window);
    if (FAILED(hr)) return hr;

    m_d3dReady = true;

    // --- Cargar Skybox ---
    std::array<std::string, 6> faces = {
        ROOT_PATH + "Skybox/cubemap_0.png", ROOT_PATH + "Skybox/cubemap_1.png", ROOT_PATH + "Skybox/cubemap_2.png",
        ROOT_PATH + "Skybox/cubemap_3.png", ROOT_PATH + "Skybox/cubemap_4.png", ROOT_PATH + "Skybox/cubemap_5.png"
    };
    m_skyboxTex.CreateCubemap(m_device, m_deviceContext, faces, false);

    // --- Actor MA5C ---
    m_cyberGun = EU::MakeShared<Actor>(m_device);

    if (!m_cyberGun.isNull()) {
        m_model = new Model3D(ROOT_PATH + "Assets/MA5C.fbx", ModelType::FBX);

        // ACTUALIZACIÓN DE RUTAS SEGÚN TU CARPETA ASSETS (Usando ROOT_PATH y sin el .png final)
        bool texError = false;
        if (FAILED(m_AlbedoSRV.init(m_device, ROOT_PATH + "Assets/MA5C_2K_Color", ExtensionType::PNG))) texError = true;
        if (FAILED(m_NormalSRV.init(m_device, ROOT_PATH + "Assets/MA5C_2K_NormalGL", ExtensionType::PNG))) texError = true;
        if (FAILED(m_MetallicSRV.init(m_device, ROOT_PATH + "Assets/MA5C_2K_Metallic", ExtensionType::PNG))) texError = true;

        // Asignamos Glossiness al slot de Roughness (común en flujos de trabajo)
        if (FAILED(m_RoughnessSRV.init(m_device, ROOT_PATH + "Assets/MA5C_2K_Glossiness", ExtensionType::PNG))) texError = true;

        // Nota: Si no tienes AO específico, puedes reusar otra o cargar una blanca
        if (FAILED(m_AOSRV.init(m_device, ROOT_PATH + "Assets/MA5C_Mg_2K_Color", ExtensionType::PNG))) texError = true;

        if (texError) {
            MESSAGE("Main", "InitDevice", "Warning: Some MA5C textures failed to load. Check paths.");
            // No retornamos E_FAIL para permitir que el motor abra y puedas ver el error en consola
        }

        std::vector<Texture> textures = { m_AlbedoSRV, m_NormalSRV, m_MetallicSRV, m_RoughnessSRV, m_AOSRV };

        m_cyberGun->setMesh(m_device, m_model->GetMeshes());
        m_cyberGun->setTextures(textures);
        m_cyberGun->setName("MA5C_AssaultRifle");
        m_actors.push_back(m_cyberGun);

        m_cyberGun->getComponent<Transform>()->setTransform(
            EU::Vector3(0.0f, 0.0f, 0.0f),
            EU::Vector3(0.0f, 0.0f, 0.0f),
            EU::Vector3(1.0f, 1.0f, 1.0f));
    }

    for (auto& actor : m_actors) m_sceneGraph.addEntity(actor.get());

    // --- Shader PorygonEngine ---
    LayoutBuilder builder;
    builder.Add("POSITION", DXGI_FORMAT_R32G32B32_FLOAT)
        .Add("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT)
        .Add("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT)
        .Add("BITANGENT", DXGI_FORMAT_R32G32B32_FLOAT)
        .Add("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);

    // Ajustamos la ruta del shader
    hr = m_shaderProgram.init(m_device, ROOT_PATH + "PorygonEngine.fx", builder);
    if (FAILED(hr)) return hr;

    hr = m_constantBuffer.init(m_device, sizeof(CBMain));
    if (FAILED(hr)) return hr;

    m_camera.setLens(XM_PIDIV4, m_window.m_width / (float)m_window.m_height, 0.01f, 1000.0f);
    m_camera.setPosition(0.0f, 1.5f, -3.0f);

    m_constantBufferStruct.LightColor = EU::Vector3(1.0f, 1.0f, 1.0f);
    m_constantBufferStruct.LightDir = EU::Vector3(-0.2f, -1.0f, 1.0f);

    m_skybox.init(m_device, &m_deviceContext, m_skyboxTex);

    hr = m_defaultRasterizer.init(m_device, D3D11_FILL_SOLID, D3D11_CULL_BACK, false, true);
    if (FAILED(hr)) return hr;

    hr = m_defaultDepthStencil.init(m_device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS);
    if (FAILED(hr)) return hr;

    hr = m_editorViewportPass.init(m_device, 1280, 720);
    if (FAILED(hr)) return hr;

    return S_OK;
}

void BaseApp::update(float deltaTime) {
    m_gui.update(m_viewport, m_window);
    m_gui.drawViewportPanel(m_editorViewportPass.getSRV());

    if (!m_actors.empty()) {
        unsigned int idx = m_gui.m_selectedActorIndex;
        if (idx < m_actors.size()) {
            m_gui.inspectorGeneral(m_actors[idx]);
            m_gui.editTransform(m_camera, m_window, m_actors[idx]);
        }
        m_gui.outliner(m_actors);
    }

    // Redimensionamiento estable
    unsigned int desiredW = static_cast<unsigned int>(m_gui.m_viewportSize.x);
    unsigned int desiredH = static_cast<unsigned int>(m_gui.m_viewportSize.y);
    const unsigned int kMinViewportSize = 64;

    if (desiredW < kMinViewportSize) desiredW = kMinViewportSize;
    if (desiredH < kMinViewportSize) desiredH = kMinViewportSize;

    if (desiredW != m_lastRequestedViewportWidth || desiredH != m_lastRequestedViewportHeight) {
        m_lastRequestedViewportWidth = desiredW;
        m_lastRequestedViewportHeight = desiredH;
        m_viewportResizeStableFrames = 0;
    }
    else {
        m_viewportResizeStableFrames++;
    }

    if (m_viewportResizeStableFrames >= 2) {
        if (desiredW != m_editorViewportPass.getWidth() || desiredH != m_editorViewportPass.getHeight()) {
            m_editorViewportResizePending = true;
            m_pendingViewportWidth = desiredW;
            m_pendingViewportHeight = desiredH;
        }
    }

    m_camera.updateViewMatrix();
    XMStoreFloat4x4(&m_constantBufferStruct.View, XMMatrixTranspose(m_camera.getView()));
    XMStoreFloat4x4(&m_constantBufferStruct.Projection, XMMatrixTranspose(m_camera.getProj()));
    m_constantBufferStruct.CameraPos = m_camera.getPosition();

    // Actualización de buffers y sistemas
    m_skybox.update(m_deviceContext, m_camera);
    m_constantBuffer.update(m_deviceContext, nullptr, 0, nullptr, &m_constantBufferStruct, 0, 0);
    m_sceneGraph.update(deltaTime, m_deviceContext);
}

void BaseApp::render() {
    handleEditorViewportResize();

    const float viewportClear[4] = { 0.10f, 0.10f, 0.10f, 1.0f };
    m_editorViewportPass.begin(m_deviceContext, viewportClear);
    m_editorViewportPass.setViewport(m_deviceContext);
    m_editorViewportPass.clearDepth(m_deviceContext);

    m_skybox.render(m_deviceContext);
    m_defaultRasterizer.render(m_deviceContext);
    m_defaultDepthStencil.render(m_deviceContext, 0, false);
    m_shaderProgram.render(m_deviceContext);
    m_constantBuffer.render(m_deviceContext, 0, 1, true);
    m_sceneGraph.render(m_deviceContext);

    // Renderizado final a la ventana
    float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    m_renderTargetView.render(m_deviceContext, m_depthStencilView, 1, ClearColor);
    m_viewport.render(m_deviceContext);

    m_gui.render();
    m_swapChain.present();
}

void BaseApp::handleEditorViewportResize() {
    if (!m_editorViewportResizePending) return;
    m_deviceContext.m_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
    ID3D11ShaderResourceView* nullSRVs[16] = {};
    m_deviceContext.m_deviceContext->PSSetShaderResources(0, 16, nullSRVs);

    EditorViewportPass newPass;
    if (SUCCEEDED(newPass.init(m_device, m_pendingViewportWidth, m_pendingViewportHeight))) {
        m_editorViewportPass.swap(newPass);
    }
    m_editorViewportResizePending = false;
}

void BaseApp::onResize(UINT newW, UINT newH) {
    if (!m_d3dReady || newW == 0 || newH == 0) return;
    m_window.m_width = (int)newW;
    m_window.m_height = (int)newH;

    m_renderTargetView.destroy();
    m_depthStencilView.destroy();
    m_depthStencil.destroy();
    m_backBuffer.destroy();

    if (SUCCEEDED(m_swapChain.resizeBuffers(newW, newH))) {
        m_swapChain.getBackBuffer(m_backBuffer);
        m_renderTargetView.init(m_device, m_backBuffer, DXGI_FORMAT_R8G8B8A8_UNORM);
        m_depthStencil.init(m_device, newW, newH, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, 4, 0);
        m_depthStencilView.init(m_device, m_depthStencil, DXGI_FORMAT_D24_UNORM_S8_UINT);
        m_viewport.init(m_window);
        m_camera.setLens(XM_PIDIV4, newW / (float)newH, 0.01f, 1000.0f);
    }
}

LRESULT BaseApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) return true;
    switch (message) {
    case WM_CREATE: {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
        return 0;
    }
    case WM_SIZE: {
        if (wParam == SIZE_MINIMIZED) return 0;
        BaseApp* app = reinterpret_cast<BaseApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (app) app->onResize(LOWORD(lParam), HIWORD(lParam));
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void BaseApp::destroy() {
    if (m_deviceContext.m_deviceContext) m_deviceContext.m_deviceContext->ClearState();
    if (m_model) { delete m_model; m_model = nullptr; }

    m_sceneGraph.destroy();
    m_editorViewportPass.destroy();
    m_AlbedoSRV.destroy(); m_NormalSRV.destroy(); m_MetallicSRV.destroy();
    m_RoughnessSRV.destroy(); m_AOSRV.destroy();
    m_defaultRasterizer.destroy();
    m_defaultDepthStencil.destroy();
    m_shaderProgram.destroy();
    m_depthStencil.destroy();
    m_depthStencilView.destroy();
    m_renderTargetView.destroy();
    m_swapChain.destroy();
    m_gui.destroy();
    m_device.destroy();
    m_backBuffer.destroy();
    m_deviceContext.destroy();
}