#pragma once

#include "Prerequisites.h"
#include "InputLayout.h"

// Forward Declarations
class Device;
class DeviceContext;

/**
 * @class ShaderProgram
 * @brief Gestiona un par de shaders (Vertex y Pixel) y su diseño de entrada.
 *
 * En DirectX 11, un "Shader Program" generalmente se refiere a la combinación
 * de un Vertex Shader (que procesa geometría) y un Pixel Shader (que procesa color/luz),
 * junto con el Input Layout que define cómo fluyen los datos hacia ellos.
 *
 * Esta clase encapsula:
 * - Compilación de código HLSL desde archivo.
 * - Creación de las interfaces ID3D11VertexShader y ID3D11PixelShader.
 * - Gestión de los blobs de memoria (bytecode).
 */
class ShaderProgram {
public:
    /**
     * @brief Constructor por defecto.
     */
    ShaderProgram() = default;

    /**
     * @brief Destructor por defecto.
     */
    ~ShaderProgram() = default;

    /**
     * @brief Inicializa el programa de shaders completo.
     *
     * Compila los shaders (VS y PS) desde el archivo especificado y crea el Input Layout
     * basado en la descripción proporcionada.
     *
     * @param device   Dispositivo para la creación de recursos.
     * @param fileName Nombre del archivo .fx o .hlsl que contiene el código fuente.
     * @param Layout   Vector que describe los elementos de entrada de los vértices (Posición, UV, Normal, etc.).
     * @return HRESULT S_OK si la compilación y creación fueron exitosas.
     */
    HRESULT init(Device& device,
        const std::string& fileName,
        std::vector<D3D11_INPUT_ELEMENT_DESC> Layout);

    /**
     * @brief Actualiza la lógica del shader (generalmente vacío).
     */
    void update();

    /**
     * @brief Activa ambos shaders (Vertex y Pixel) y el Input Layout en el pipeline.
     *
     * @param deviceContext Contexto del dispositivo.
     */
    void render(DeviceContext& deviceContext);

    /**
     * @brief Activa un tipo específico de shader (solo VS o solo PS).
     *
     * @param deviceContext Contexto del dispositivo.
     * @param type          Tipo de shader a activar (VERTEX_SHADER o PIXEL_SHADER).
     */
    void render(DeviceContext& deviceContext, ShaderType type);

    /**
     * @brief Libera los recursos de shaders, blobs y el input layout.
     */
    void destroy();

    // ------------------------------------------------------------------------
    // Métodos de Creación y Compilación (Helpers)
    // ------------------------------------------------------------------------

    /**
     * @brief Crea el objeto Input Layout.
     *
     * @param device Dispositivo DirectX.
     * @param Layout Vector de descripción de elementos.
     * @return HRESULT S_OK si tiene éxito.
     */
    HRESULT CreateInputLayout(Device& device,
        std::vector<D3D11_INPUT_ELEMENT_DESC> Layout);

    /**
     * @brief Crea un shader específico (VS o PS) usando el nombre de archivo interno.
     *
     * Asume puntos de entrada estándar ("VS" para Vertex, "PS" para Pixel) y
     * modelos de shader ("vs_4_0", "ps_4_0").
     *
     * @param device Dispositivo DirectX.
     * @param type   Tipo de shader a crear.
     * @return HRESULT S_OK si la compilación tiene éxito.
     */
    HRESULT CreateShader(Device& device, ShaderType type);

    /**
     * @brief Crea un shader específico desde un archivo explícito.
     *
     * @param device   Dispositivo DirectX.
     * @param type     Tipo de shader a crear.
     * @param fileName Ruta del archivo HLSL.
     * @return HRESULT S_OK si la compilación tiene éxito.
     */
    HRESULT CreateShader(Device& device, ShaderType type, const std::string& fileName);

    /**
     * @brief Función de bajo nivel que envuelve D3DCompileFromFile.
     *
     * @param szFileName    Ruta del archivo HLSL.
     * @param szEntryPoint  Nombre de la función principal en el HLSL (ej. "main", "VS", "PS").
     * @param szShaderModel Versión del compilador (ej. "vs_4_0", "ps_5_0").
     * @param ppBlobOut     Salida: Blob binario con el código compilado.
     * @return HRESULT      S_OK si compila, o error si falla (imprime errores en el Output de VS).
     */
    HRESULT CompileShaderFromFile(char* szFileName,
        LPCSTR szEntryPoint,
        LPCSTR szShaderModel,
        ID3DBlob** ppBlobOut);

public:
    /**
     * @brief Interfaz del Vertex Shader. Procesa cada vértice de la malla.
     */
    ID3D11VertexShader* m_VertexShader = nullptr;

    /**
     * @brief Interfaz del Pixel Shader. Calcula el color final de cada píxel.
     */
    ID3D11PixelShader* m_PixelShader = nullptr;

    /**
     * @brief Objeto que define el formato de los datos de vértices.
     */
    InputLayout m_inputLayout;

private:
    std::string m_shaderFileName;       ///< Ruta del archivo fuente HLSL.

    ID3DBlob* m_vertexShaderData = nullptr; ///< Bytecode compilado del VS (necesario para el Input Layout).
    ID3DBlob* m_pixelShaderData = nullptr;  ///< Bytecode compilado del PS.
};