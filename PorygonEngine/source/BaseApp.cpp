#include "BaseApp.h"
#include "ResourceManager.h" // Asegurate de que esto exista o quitalo si no se usa

HRESULT BaseApp::awake() {
    HRESULT hr = S_OK;
    m_sceneGraph.init();
    MESSAGE("Main", "Awake", "Application awake successfully.");
    return hr;
}

int BaseApp::run(HINSTANCE hInst, int nCmdShow) {
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

    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
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

HRESULT BaseApp::init() {
    HRESULT hr = S_OK;

    // --- PIPELINE INIT ---
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

    // --- CAMERA INIT ---
    // Posicion inicial y Lente
    m_camera.setPosition(0.0f, 1.5f, -3.0f);
    m_camera.setLens(XM_PIDIV4, m_window.m_width / (FLOAT)m_window.m_height, 0.01f, 1000.0f);
    m_camera.updateViewMatrix(); // Generar la primera matriz de vista

    // --- SKYBOX ---
    std::array<std::string, 6> faces = {
        "Skybox/cubemap_0.png", "Skybox/cubemap_1.png", "Skybox/cubemap_2.png",
        "Skybox/cubemap_3.png", "Skybox/cubemap_4.png", "Skybox/cubemap_5.png"
    };
    m_skyboxTex.CreateCubemap(m_device, m_deviceContext, faces, true);

    // --- ACTOR 1: CYBERGUN ---
    m_cyberGun = EU::MakeShared<Actor>(m_device);
    if (!m_cyberGun.isNull()) {
        m_model = new Model3D("Assets/MA5C.fbx", ModelType::FBX);
        hr = m_cyberGunAlbedo.init(m_device, "Assets/MA5C_2K_Color", ExtensionType::PNG);

        if (SUCCEEDED(hr)) {
            m_cyberGun->setMesh(m_device, m_model->GetMeshes());
            std::vector<Texture> textures;
            textures.push_back(m_cyberGunAlbedo);
            m_cyberGun->setTextures(textures);

            m_cyberGun->setName("CyberGun");
            m_actors.push_back(m_cyberGun);

            // Transformación inicial
            m_cyberGun->getComponent<Transform>()->setTransform(
                EU::Vector3(0, 0, 0), EU::Vector3(0, 0, 0), EU::Vector3(1, 1, 1));
        }
    }

    // --- ACTOR 2: CHARACTER (Ahora sí está definido en .h) ---
    m_Character = EU::MakeShared<Actor>(m_device);
    m_Character->setName("Character_Porygon");
    m_Character->getComponent<Transform>()->setTransform(
        EU::Vector3(2.0f, -4.90f, 11.60f),
        EU::Vector3(-0.60f, 3.0f, -0.20f),
        EU::Vector3(1.0f, 1.0f, 1.0f)
    );
    // Agregamos a la lista general si quieres que salga en el outliner
    m_actors.push_back(m_Character);

    // --- SCENE GRAPH ---
    // Añadimos las entidades base
    if (!m_actors.empty()) {
        m_sceneGraph.addEntity(m_actors[0].get()); // CyberGun como raíz ejemplo
    }

    // Adjuntar Character a CyberGun (ejemplo de jerarquía)
    // O simplemente añadirlo como entidad independiente:
    if (m_cyberGun) {
        m_sceneGraph.attach(m_Character.get(), m_cyberGun.get());
    }
    else {
        m_sceneGraph.addEntity(m_Character.get());
    }

    // --- SHADER SETUP ---
    std::vector<D3D11_INPUT_ELEMENT_DESC> Layout = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = m_shaderProgram.init(m_device, "PorygonEngine.fx", Layout);
    if (FAILED(hr)) return hr;

    hr = m_cbNeverChanges.init(m_device, sizeof(CBNeverChanges));
    hr = m_cbChangeOnResize.init(m_device, sizeof(CBChangeOnResize));

    return hr;
}

void BaseApp::update(float deltaTime) {
    // 1. UPDATE CAMERA
    // Aquí puedes meter input: if(Key('W')) m_camera.walk(deltaTime);
    m_camera.updateViewMatrix();

    // 2. UPDATE UI
    m_gui.update(m_viewport, m_window);

    // Inspector y Outliner
    if (!m_actors.empty()) {
        // Asegúrate de que selectedActorIndex sea válido en tu GUI
        if (m_gui.selectedActorIndex < m_actors.size())
            m_gui.inspectorGeneral(m_actors[m_gui.selectedActorIndex]);

        m_gui.outliner(m_actors);
    }

    // --- Debug Skybox UI ---
    static ID3D11ShaderResourceView* faceSRV[6] = { nullptr };
    if (!faceSRV[0]) {
        for (UINT i = 0; i < 6; ++i) {
            faceSRV[i] = m_skyboxTex.CreateCubemapFaceSRV(m_device.m_device, m_skyboxTex.m_texture,
                DXGI_FORMAT_R8G8B8A8_UNORM, i, 1);
        }
    }
    // (Opcional: Código ImGui del skybox...)

    // 3. ACTUALIZAR CONSTANT BUFFERS
    // Usamos m_camera.getView() y getProj() en lugar de m_View/m_Projection
    cbNeverChanges.mView = XMMatrixTranspose(m_camera.getView());
    m_cbNeverChanges.update(m_deviceContext, nullptr, 0, nullptr, &cbNeverChanges, 0, 0);

    cbChangesOnResize.mProjection = XMMatrixTranspose(m_camera.getProj());
    m_cbChangeOnResize.update(m_deviceContext, nullptr, 0, nullptr, &cbChangesOnResize, 0, 0);

    // 4. UPDATE SCENE
    m_sceneGraph.update(deltaTime, m_deviceContext);

    // 5. GIZMOS
    if (!m_actors.empty() && m_gui.selectedActorIndex < m_actors.size()) {
        // Pasamos las matrices de la cámara al Gizmo
        m_gui.editTransform(m_camera.getView(), m_camera.getProj(), m_actors[m_gui.selectedActorIndex]);
    }
}

void BaseApp::render() {
    float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    m_renderTargetView.render(m_deviceContext, m_depthStencilView, 1, ClearColor);

    m_viewport.render(m_deviceContext);
    m_depthStencilView.render(m_deviceContext);
    m_shaderProgram.render(m_deviceContext);

    m_cbNeverChanges.render(m_deviceContext, 0, 1);
    m_cbChangeOnResize.render(m_deviceContext, 1, 1);

    m_sceneGraph.render(m_deviceContext);
    m_gui.render();

    m_swapChain.present();
}

void BaseApp::destroy() {
    if (m_deviceContext.m_deviceContext) m_deviceContext.m_deviceContext->ClearState();

    if (m_model) {
        delete m_model;
        m_model = nullptr;
    }

    // Destrucción ordenada
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

LRESULT BaseApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) {
        return true;
    }

    switch (message) {
    case WM_CREATE:
    {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
    }
    return 0;
    case WM_PAINT:
    {
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