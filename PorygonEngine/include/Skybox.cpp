#include "EngineUtilities/Utilities/Skybox.h"
#include "Device.h"
#include "DeviceContext.h"

HRESULT
Skybox::init(Device& device, DeviceContext* deviceContext, Texture& cubemap) {
    // 0) Limpiar si ya existía algo
    destroy();

    // Cargar el cubemap
    m_skyboxTexture = cubemap;

    // 1) Geometría (cubo unitario)
    const SkyboxVertex vertices[] = {
        {-1,-1,-1}, {-1,+1,-1}, {+1,+1,-1}, {+1,-1,-1}, // back
        {-1,-1,+1}, {-1,+1,+1}, {+1,+1,+1}, {+1,-1,+1}, // front
    };

    const unsigned int indices[] = {
        0,1,2, 0,2,3, // back (-Z)
        4,6,5, 4,7,6, // front (+Z)
        4,5,1, 4,1,0, // left (-X)
        3,2,6, 3,6,7, // right (+X)
        1,5,6, 1,6,2, // top (+Y)
        4,0,3, 4,3,7  // bottom (-Y)
    };

    // 2) Load Model
    m_skybox = EU::MakeShared<Actor>(device);

    if (!m_skybox.isNull()) {
        std::vector<MeshComponent> skyboxMeshes;
        m_cubeModel = new Model3D("Skybox", vertices, indices);
        skyboxMeshes = m_cubeModel->GetMeshes();

        m_skybox->setMesh(device, skyboxMeshes);
        m_skybox->setName("SkyboxActor");
    }
    else {
        ERROR("Skybox", "Init", "Failed to create Skybox Actor.");
        return E_FAIL;
    }

    // 3) Define Input Layout
    std::vector<D3D11_INPUT_ELEMENT_DESC> Layout;
    D3D11_INPUT_ELEMENT_DESC position;
    position.SemanticName = "POSITION";
    position.SemanticIndex = 0;
    position.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    position.InputSlot = 0;
    position.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    position.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    position.InstanceDataStepRate = 0;
    Layout.push_back(position);

    HRESULT hr = S_OK;

    // 4) Shaders
    hr = m_shaderProgram.init(device, "Skybox.fx", Layout);
    if (FAILED(hr)) return hr;

    // 5) Constant Buffer (CBSkybox)
    hr = m_constantBuffer.init(device, sizeof(CBSkybox));
    if (FAILED(hr)) {
        ERROR("Skybox", "init", ("Failed to initialize Skybox Constant Buffer. HRESULT: " + std::to_string(hr)).c_str());
        return hr;
    }

    // 6) Sampler State
    hr = m_samplerState.init(device);
    if (FAILED(hr)) {
        ERROR("Skybox", "init", "Failed to create new SamplerState");
        return hr;
    }

    return S_OK; // IMPORTANTE: Cambiado de E_NOTIMPL a S_OK
}

void
Skybox::update() {
    // Reservado para lógica de rotación del cielo si fuera necesario
}

void
Skybox::render(DeviceContext& deviceContext, Camera& camera) {
    if (!m_cubeModel) return;

    // 1) View sin traslación para que el Skybox siempre rodee a la cámara
    // Obtenemos la matriz de vista y quitamos la parte de movimiento
    XMMATRIX view = camera.getView();
    // Dependiendo de tu cámara, esto anula la posición
    view.r[3] = XMVectorSet(0, 0, 0, 1);

    XMMATRIX vp = view * camera.getProj();

    // 2) Actualizar Constant Buffer
    CBSkybox cb{};
    cb.mviewProj = XMMatrixTranspose(vp);
    m_constantBuffer.update(deviceContext, nullptr, 0, nullptr, &cb, 0, 0);

    // 3) Bind de recursos
    m_constantBuffer.render(deviceContext, 0, 1);
    m_shaderProgram.render(deviceContext);
    m_samplerState.render(deviceContext, 0, 1);
    m_skyboxTexture.render(deviceContext, 0, 1);

    // 4) Draw (Cubo)
    deviceContext.DrawIndexed(m_cubeModel->m_meshes[0].m_index.size(), 0, 0);
}

void
Skybox::destroy() {
    // Liberar memoria del modelo 3D
    if (m_cubeModel) {
        delete m_cubeModel;
        m_cubeModel = nullptr;
    }

    // Llamar a los destroy de los componentes del motor
    m_shaderProgram.destroy();
    m_constantBuffer.destroy();
    m_samplerState.destroy();
    m_skyboxTexture.destroy();

    // El TSharedPointer de m_skybox se limpia automáticamente
}