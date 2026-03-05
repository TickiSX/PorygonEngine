#include "BaseApp.h"
#include "ResourceManager.h"

HRESULT
BaseApp::awake() {
    HRESULT hr = S_OK;

    // Inicializacion de dlls y elementos externos al motor.
    m_sceneGraph.init();

    // Log Success Message
    MESSAGE("Main", "Awake", "Application awake successfully.");
    return hr;
}

int
BaseApp::run(HINSTANCE hInst, int nCmdShow) {
    // 1) Initialize Window
    if (FAILED(m_window.init(hInst, nCmdShow, WndProc))) {
        ERROR("Main", "Run", "Failed to initialize window.");
        return 0;
    }
    // 2) Awake Application
    if (FAILED(awake())) {
        ERROR("Main", "Run", "Failed to awake application.");
        return 0;
    }
    // 3) Initialize Device and Device Context
    if (FAILED(init())) {
        ERROR("Main", "Run", "Failed to initialize device and device context.");
        return 0;
    }
    // 4) Initialize GUI
    m_gui.init(m_window, m_device, m_deviceContext);

    // Main message loop
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

HRESULT
BaseApp::init() {
    HRESULT hr = S_OK;

    // Crear swapchain
    hr = m_swapChain.init(m_device, m_deviceContext, m_backBuffer, m_window);
    if (FAILED(hr)) {
        ERROR("Main", "InitDevice", ("Failed to initialize SwapChain. HRESULT: " + std::to_string(hr)).c_str());
        return hr;
    }

    // Crear render target view
    hr = m_renderTargetView.init(m_device, m_backBuffer, DXGI_FORMAT_R8G8B8A8_UNORM);
    if (FAILED(hr)) {
        ERROR("Main", "InitDevice", ("Failed to initialize RenderTargetView. HRESULT: " + std::to_string(hr)).c_str());
        return hr;
    }

    // Crear textura de depth stencil
    hr = m_depthStencil.init(m_device, m_window.m_width, m_window.m_height,
        DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, 4, 0);
    if (FAILED(hr)) {
        ERROR("Main", "InitDevice", ("Failed to initialize DepthStencil. HRESULT: " + std::to_string(hr)).c_str());
        return hr;
    }

    // Crear el depth stencil view
    hr = m_depthStencilView.init(m_device, m_depthStencil, DXGI_FORMAT_D24_UNORM_S8_UINT);
    if (FAILED(hr)) {
        ERROR("Main", "InitDevice", ("Failed to initialize DepthStencilView. HRESULT: " + std::to_string(hr)).c_str());
        return hr;
    }

    // Crear el viewport
    hr = m_viewport.init(m_window);
    if (FAILED(hr)) {
        ERROR("Main", "InitDevice", ("Failed to initialize Viewport. HRESULT: " + std::to_string(hr)).c_str());
        return hr;
    }

    // --- Cargar Recursos (Skybox) ---
    std::array<std::string, 6> faces = {
        "Skybox/cubemap_0.png", "Skybox/cubemap_1.png", "Skybox/cubemap_2.png",
        "Skybox/cubemap_3.png", "Skybox/cubemap_4.png", "Skybox/cubemap_5.png"
    };
    m_skyboxTex.CreateCubemap(m_device, m_deviceContext, faces, false);

    // --- Set CyberGun Actor (Manteniendo MA5C) ---
    m_cyberGun = EU::MakeShared<Actor>(m_device);

    if (!m_cyberGun.isNull()) {
        m_model = new Model3D("Assets/MA5C.fbx", ModelType::FBX);

        // Carga de textura MA5C
        hr = m_cyberGunAlbedo.init(m_device, "Assets/MA5C_2K_Color", ExtensionType::PNG);
        if (FAILED(hr)) {
            ERROR("Main", "InitDevice", ("Failed to initialize MA5C textures. HRESULT: " + std::to_string(hr)).c_str());
            return hr;
        }

        m_cyberGun->setMesh(m_device, m_model->GetMeshes());

        std::vector<Texture> textures;
        textures.push_back(m_cyberGunAlbedo);
        m_cyberGun->setTextures(textures);

        m_cyberGun->setName("CyberGun_MA5C");
        m_actors.push_back(m_cyberGun);

        // Transformación inicial
        m_cyberGun->getComponent<Transform>()->setTransform(
            EU::Vector3(0.0f, 0.0f, 0.0f),
            EU::Vector3(0.0f, 0.0f, 0.0f),
            EU::Vector3(1.0f, 1.0f, 1.0f));
    }
    else {
        ERROR("Main", "InitDevice", "Failed to create cyber Gun Actor.");
        return E_FAIL;
    }

    // Registrar actores en el Grafo
    for (auto& actor : m_actors) {
        m_sceneGraph.addEntity(actor.get());
    }

    // Definir Input Layout
    std::vector<D3D11_INPUT_ELEMENT_DESC> Layout;
    Layout.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
    Layout.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
    Layout.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });

    // Crear Shader Program (Manteniendo PorygonEngine.fx)
    hr = m_shaderProgram.init(m_device, "PorygonEngine.fx", Layout);
    if (FAILED(hr)) {
        ERROR("Main", "InitDevice", ("Failed to initialize PorygonEngine.fx. HRESULT: " + std::to_string(hr)).c_str());
        return hr;
    }

    // Crear Constant Buffers con validación
    hr = m_cbNeverChanges.init(m_device, sizeof(CBNeverChanges));
    if (FAILED(hr)) return hr;

    hr = m_cbChangeOnResize.init(m_device, sizeof(CBChangeOnResize));
    if (FAILED(hr)) return hr;

    // Inicializar Camara
    m_camera.setLens(XM_PIDIV4, m_window.m_width / (float)m_window.m_height, 0.01f, 1000.0f);
    m_camera.setPosition(0.0f, 1.5f, -3.0f);
    m_camera.updateViewMatrix();

    cbNeverChanges.mView = XMMatrixTranspose(m_camera.getView());
    cbChangesOnResize.mProjection = XMMatrixTranspose(m_camera.getProj());

    // Inicializar Skybox pass
    m_skybox.init(m_device, &m_deviceContext, m_skyboxTex);

    // Inicializar estados por defecto
    hr = m_defaultRasterizer.init(m_device, D3D11_FILL_SOLID, D3D11_CULL_BACK, false, true);
    if (FAILED(hr)) return hr;

    hr = m_defaultDepthStencil.init(m_device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS);
    if (FAILED(hr)) return hr;

    return S_OK;
}

void BaseApp::update(float deltaTime)
{
    // Actualizar Interfaz
    m_gui.update(m_viewport, m_window);

    // Lógica de selección segura para GUI e Inspector
    if (!m_actors.empty()) {
        unsigned int idx = m_gui.selectedActorIndex;
        if (idx < m_actors.size()) {
            m_gui.inspectorGeneral(m_actors[idx]);
            m_gui.editTransform(m_camera.getView(), m_camera.getProj(), m_actors[idx]);
        }
        m_gui.outliner(m_actors);
    }

    // Actualizar Matrices de Camara y Buffers
    m_camera.updateViewMatrix();
    cbNeverChanges.mView = XMMatrixTranspose(m_camera.getView());
    m_cbNeverChanges.update(m_deviceContext, nullptr, 0, nullptr, &cbNeverChanges, 0, 0);

    cbChangesOnResize.mProjection = XMMatrixTranspose(m_camera.getProj());
    m_cbChangeOnResize.update(m_deviceContext, nullptr, 0, nullptr, &cbChangesOnResize, 0, 0);

    // Actualizar Grafo de Escena
    m_sceneGraph.update(deltaTime, m_deviceContext);
}

void
BaseApp::render() {
    float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    m_renderTargetView.render(m_deviceContext, m_depthStencilView, 1, ClearColor);

    m_viewport.render(m_deviceContext);
    m_depthStencilView.render(m_deviceContext);

    // 1) SKYBOX PASS
    m_skybox.render(m_deviceContext, m_camera);

    // 2) RESTAURAR ESTADOS + PIPELINE DE ESCENA
    m_defaultRasterizer.render(m_deviceContext);
    m_defaultDepthStencil.render(m_deviceContext, 0, false);

    // Limpieza de SRVs por seguridad
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    m_deviceContext.m_deviceContext->PSSetShaderResources(10, 1, nullSRV);
    m_deviceContext.m_deviceContext->PSSetShaderResources(0, 1, nullSRV);

    // Re-bindea shader/layout de escena
    m_shaderProgram.render(m_deviceContext);

    // CBs para VS (view/proj)
    m_cbNeverChanges.render(m_deviceContext, 0, 1);
    m_cbChangeOnResize.render(m_deviceContext, 1, 1);

    // 3) SCENE PASS
    m_sceneGraph.render(m_deviceContext);

    // 4) GUI
    m_gui.render();

    m_swapChain.present();
}

void
BaseApp::destroy() {
    if (m_deviceContext.m_deviceContext) m_deviceContext.m_deviceContext->ClearState();

    if (m_model) {
        delete m_model;
        m_model = nullptr;
    }

    m_sceneGraph.destroy();
    m_cbNeverChanges.destroy();
    m_cbChangeOnResize.destroy();
    m_shaderProgram.destroy();
    m_depthStencil.destroy();
    m_depthStencilView.destroy();
    m_renderTargetView.destroy();
    m_swapChain.destroy();
    m_backBuffer.destroy();
    m_gui.destroy();
    m_deviceContext.destroy();
    m_device.destroy();
}

LRESULT
BaseApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) {
        return true;
    }

    switch (message) {
    case WM_CREATE: {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
    }
                  return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
                 return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}