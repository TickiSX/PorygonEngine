#pragma once

#include "Prerequisites.h"

/**
 * @class Device
 * @brief Encapsula la interfaz ID3D11Device de DirectX 11.
 *
 * En la arquitectura de DirectX 11, el "Device" actúa como una fábrica de recursos.
 * Es responsable de verificar las capacidades del hardware (adaptador de video) y
 * de asignar memoria para crear recursos como Texturas, Buffers, Shaders y Vistas.
 *
 * A diferencia del DeviceContext (que envía comandos de renderizado), el Device
 * es "thread-safe" (seguro para hilos) para la creación de recursos.
 */
class Device {
public:
    /**
     * @brief Constructor por defecto.
     */
    Device() = default;

    /**
     * @brief Destructor por defecto.
     */
    ~Device() = default;

    /**
     * @brief Inicializa el dispositivo y la cadena de intercambio (si aplica).
     * Configura los flags de creación (ej. modo Debug) y selecciona el driver.
     */
    void init();

    /**
     * @brief Actualiza la lógica del dispositivo (generalmente vacío para esta clase).
     */
    void update();

    /**
     * @brief Realiza operaciones de renderizado o presentación (si aplica).
     */
    void render();

    /**
     * @brief Libera el recurso ID3D11Device y limpia la memoria.
     */
    void destroy();

    // ------------------------------------------------------------------------
    // Métodos de Creación de Recursos (Factory Methods)
    // ------------------------------------------------------------------------

    /**
     * @brief Crea una vista de Render Target (RTV).
     * Permite usar una textura (como el Back Buffer) como destino de dibujo.
     *
     * @param pResource  Puntero al recurso (Textura) que recibirá el renderizado.
     * @param pDesc      Descripción de la vista (puede ser NULL para usar el formato del recurso).
     * @param ppRTView   Salida: Puntero doble donde se almacenará la vista creada.
     * @return S_OK si la operación fue exitosa.
     */
    HRESULT CreateRenderTargetView(ID3D11Resource* pResource,
        const D3D11_RENDER_TARGET_VIEW_DESC* pDesc,
        ID3D11RenderTargetView** ppRTView);

    /**
     * @brief Crea una Textura 2D.
     * Usado para Depth Buffers, texturas de modelos o superficies de renderizado.
     *
     * @param pDesc        Estructura que describe el tamaño, formato y uso de la textura.
     * @param pInitialData Datos iniciales (píxeles) para llenar la textura (opcional).
     * @param ppTexture2D  Salida: Puntero doble donde se almacenará la textura creada.
     * @return S_OK si la operación fue exitosa.
     */
    HRESULT CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc,
        const D3D11_SUBRESOURCE_DATA* pInitialData,
        ID3D11Texture2D** ppTexture2D);

    /**
     * @brief Crea una vista de Profundidad/Stencil (DSV).
     * Permite usar una textura como buffer Z (Depth Buffer).
     *
     * @param pResource          Recurso de textura que servirá como buffer de profundidad.
     * @param pDesc              Descripción de la vista.
     * @param ppDepthStencilView Salida: Puntero doble donde se almacenará la vista creada.
     * @return S_OK si la operación fue exitosa.
     */
    HRESULT CreateDepthStencilView(ID3D11Resource* pResource,
        const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc,
        ID3D11DepthStencilView** ppDepthStencilView);

    /**
     * @brief Crea un Vertex Shader desde bytecode compilado.
     *
     * @param pShaderBytecode Puntero al blob de datos del shader compilado.
     * @param BytecodeLength  Tamaño en bytes del shader.
     * @param pClassLinkage   Enlace de clases para shaders dinámicos (usualmente nullptr).
     * @param ppVertexShader  Salida: Puntero al Vertex Shader creado.
     * @return S_OK si la operación fue exitosa.
     */
    HRESULT CreateVertexShader(const void* pShaderBytecode,
        unsigned int BytecodeLength,
        ID3D11ClassLinkage* pClassLinkage,
        ID3D11VertexShader** ppVertexShader);

    /**
     * @brief Crea un Input Layout (Diseño de Entrada).
     * Define cómo los datos del Vertex Buffer se mapean a las variables del Vertex Shader.
     *
     * @param pInputElementDescs Array de descripciones de elementos (Semántica, formato, etc.).
     * @param NumElements        Número de elementos en el array.
     * @param pShaderBytecodeWithInputSignature Bytecode del VS para validar la firma.
     * @param BytecodeLength     Tamaño del bytecode.
     * @param ppInputLayout      Salida: Puntero al Input Layout creado.
     * @return S_OK si la operación fue exitosa.
     */
    HRESULT CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
        UINT NumElements,
        const void* pShaderBytecodeWithInputSignature,
        unsigned int BytecodeLength,
        ID3D11InputLayout** ppInputLayout);

    /**
     * @brief Crea un Pixel Shader desde bytecode compilado.
     *
     * @param pShaderBytecode Puntero al blob de datos del shader.
     * @param BytecodeLength  Tamaño en bytes.
     * @param pClassLinkage   Usualmente nullptr.
     * @param ppPixelShader   Salida: Puntero al Pixel Shader creado.
     * @return S_OK si la operación fue exitosa.
     */
    HRESULT CreatePixelShader(const void* pShaderBytecode,
        unsigned int BytecodeLength,
        ID3D11ClassLinkage* pClassLinkage,
        ID3D11PixelShader** ppPixelShader);

    /**
     * @brief Crea un Buffer (Vertex, Index o Constant).
     *
     * @param pDesc        Descripción del buffer (Uso, Tamaño, Flags de unión).
     * @param pInitialData Datos iniciales para llenar el buffer (opcional).
     * @param ppBuffer     Salida: Puntero al Buffer creado.
     * @return S_OK si la operación fue exitosa.
     */
    HRESULT CreateBuffer(const D3D11_BUFFER_DESC* pDesc,
        const D3D11_SUBRESOURCE_DATA* pInitialData,
        ID3D11Buffer** ppBuffer);

    /**
     * @brief Crea un Estado de Muestreo (Sampler State).
     * Controla cómo se leen y filtran las texturas (Lineal, Punto, Anisótropico, Wrapping).
     *
     * @param pSamplerDesc   Descripción de los filtros y modos de direccionamiento.
     * @param ppSamplerState Salida: Puntero al Sampler State creado.
     * @return S_OK si la operación fue exitosa.
     */
    HRESULT CreateSamplerState(const D3D11_SAMPLER_DESC* pSamplerDesc,
        ID3D11SamplerState** ppSamplerState);

public:
    /**
     * @brief Puntero nativo a la interfaz del dispositivo DirectX 11.
     * Se mantiene público para facilitar el acceso desde otras clases amigas o sistemas.
     */
    ID3D11Device* m_device = nullptr;
};