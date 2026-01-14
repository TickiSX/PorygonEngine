#include "BaseApp.h"
#include "ResourceManager.h"

HRESULT
BaseApp::awake() {
    HRESULT hr = S_OK;

    // Inicialización de elementos externos al motor
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

    // 4) Initialize GUI (Usando m_gui y pasando m_window)
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

    // --- Core Pipeline ---
    hr = m_swapChain.init(m_device, m_deviceContext, m_backBuffer, m_window);
    if (FAILED(hr)) {
        ERROR("Main", "InitDevice", ("Failed to initialize SwapChain. HRESULT: " + std::to_string(hr)).c_str());
        return hr;
    }

    hr = m_renderTargetView.init(m_device, m_backBuffer, DXGI_FORMAT_R8G8B8A8_UNORM);
    if (FAILED(hr)) {
        ERROR("Main", "InitDevice", ("Failed to initialize RenderTargetView. HRESULT: " + std::to_string(hr)).c_str());
        return hr;
    }

    hr = m_depthStencil.init(m_device, m_window.m_width, m_window.m_height, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, 4, 0);
    if (FAILED(hr)) {
        ERROR("Main", "InitDevice", ("Failed to initialize DepthStencil. HRESULT: " + std::to_string(hr)).c_str());
        return hr;
    }

    hr = m_depthStencilView.init(m_device, m_depthStencil, DXGI_FORMAT_D24_UNORM_S8_UINT);
    if (FAILED(hr)) {
        ERROR("Main", "InitDevice", ("Failed to initialize DepthStencilView. HRESULT: " + std::to_string(hr)).c_str());
        return hr;
    }

    hr = m_viewport.init(m_window);
    if (FAILED(hr)) {
        ERROR("Main", "InitDevice", ("Failed to initialize Viewport. HRESULT: " + std::to_string(hr)).c_str());
        return hr;
    }

    // --- CARGA DE RECURSOS (Actor CyberGun) ---
    m_cyberGun = EU::MakeShared<Actor>(m_device);
    if (!m_cyberGun.isNull()) {
        m_model = new Model3D("Assets/MA5C.fbx", ModelType::FBX);
        std::vector<MeshComponent> cyberGunMeshes = m_model->GetMeshes();

        std::vector<Texture> cyberGunTextures;
        hr = m_cyberGunAlbedo.init(m_device, "Assets/MA5C_2K_Color", ExtensionType::PNG);
        if (FAILED(hr)) {
            ERROR("Main", "InitDevice", "Failed to initialize cyberGunAlbedo.");
            return hr;
        }
        cyberGunTextures.push_back(m_cyberGunAlbedo);

        m_cyberGun->setMesh(m_device, cyberGunMeshes);
        m_cyberGun->setTextures(cyberGunTextures);
        m_cyberGun->setName("CyberGun");
        m_actors.push_back(m_cyberGun);

        // Transformación inicial
        m_cyberGun->getComponent<Transform>()->setTransform(
            EU::Vector3(0.0f, 0.0f, 0.0f),
            EU::Vector3(0.0f, 0.0f, 0.0f),
            EU::Vector3(1.0f, 1.0f, 1.0f)
        );
    }
    else {
        ERROR("Main", "InitDevice", "Failed to create cyber Gun Actor.");
        return E_FAIL;
    }

    // --- SHADERS Y LAYOUT ---
    std::vector<D3D11_INPUT_ELEMENT_DESC> Layout = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    hr = m_shaderProgram.init(m_device, "PorygonEngine.fx", Layout);
    if (FAILED(hr)) return hr;

    hr = m_cbNeverChanges.init(m_device, sizeof(CBNeverChanges));
    hr = m_cbChangeOnResize.init(m_device, sizeof(CBChangeOnResize));

    // --- CÁMARA ---
    XMVECTOR Eye = XMVectorSet(0.0f, 1.5f, -3.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    m_View = XMMatrixLookAtLH(Eye, At, Up);

    m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_window.m_width / (FLOAT)m_window.m_height, 0.01f, 100.0f);

    return S_OK;
}

void BaseApp::update(float deltaTime) {
    // 1. Update Time
    static float t = 0.0f;
    static DWORD dwTimeStart = 0;
    DWORD dwTimeCur = GetTickCount();
    if (dwTimeStart == 0) dwTimeStart = dwTimeCur;
    t = (dwTimeCur - dwTimeStart) / 1000.0f;

    // 2. GUI Update & Logic
    m_gui.update(m_window);

    // Solo procesamos UI si hay actores
    if (!m_actors.empty()) {
        m_gui.outliner(m_actors);
        m_gui.inspectorGeneral(m_actors[m_gui.selectedActorIndex]);

        // ImGuizmo se llama dentro de la lógica de update (o antes del render de UI)
        m_gui.editTransform(m_View, m_Projection, m_actors[m_gui.selectedActorIndex]);
    }

    // 3. Update Constant Buffers
    cbNeverChanges.mView = XMMatrixTranspose(m_View);
    m_cbNeverChanges.update(m_deviceContext, nullptr, 0, nullptr, &cbNeverChanges, 0, 0);

    m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_window.m_width / (FLOAT)m_window.m_height, 0.01f, 100.0f);
    cbChangesOnResize.mProjection = XMMatrixTranspose(m_Projection);
    m_cbChangeOnResize.update(m_deviceContext, nullptr, 0, nullptr, &cbChangesOnResize, 0, 0);

    // 4. Update Actors
    for (auto& actor : m_actors) {
        actor->update(deltaTime, m_deviceContext);
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

    for (auto& actor : m_actors) {
        actor->render(m_deviceContext);
    }

    // Render UI final
    m_gui.render();

    m_swapChain.present();
}

void BaseApp::destroy() {
    if (m_deviceContext.m_deviceContext) m_deviceContext.m_deviceContext->ClearState();
    if (m_model) delete m_model;

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
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message) {
    case WM_CREATE: {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
    } return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    } return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}