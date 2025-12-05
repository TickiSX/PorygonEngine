#pragma once

#include "Prerequisites.h"

// Forward Declarations
class Device;
class DeviceContext;

/**
 * @class Texture
 * @brief Representa una textura 2D y sus vistas asociadas en DirectX 11.
 *
 * Esta clase encapsula el recurso físico (ID3D11Texture2D) y su vista de recurso
 * para shaders (ID3D11ShaderResourceView). Permite:
 * 1. Cargar imágenes desde disco (PNG, JPG, DDS).
 * 2. Crear texturas vacías (para Render Targets o Depth Buffers).
 * 3. Envolver texturas existentes (como el Back Buffer).
 */
class Texture {
public:
    /**
     * @brief Constructor por defecto.
     */
    Texture() = default;

    /**
     * @brief Destructor por defecto.
     */
    ~Texture() = default;

    /**
     * @brief Inicializa la textura cargando una imagen desde disco.
     *
     * @param device        Referencia al dispositivo para la creación del recurso.
     * @param textureName   Ruta o nombre del archivo de imagen.
     * @param extensionType Tipo de archivo (DDS, PNG, JPG).
     * @return HRESULT      S_OK si la carga fue exitosa.
     */
    HRESULT init(Device& device,
        const std::string& textureName,
        ExtensionType extensionType);

    /**
     * @brief Inicializa una textura vacía en memoria (procedural o para render target).
     *
     * @param device        Referencia al dispositivo.
     * @param width         Ancho de la textura en píxeles.
     * @param height        Alto de la textura en píxeles.
     * @param Format        Formato de los datos (ej. DXGI_FORMAT_R8G8B8A8_UNORM).
     * @param BindFlags     Banderas de uso (ej. D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE).
     * @param sampleCount   Número de muestras MSAA (por defecto 1).
     * @param qualityLevels Niveles de calidad MSAA (por defecto 0).
     * @return HRESULT      S_OK si la creación fue exitosa.
     */
    HRESULT init(Device& device,
        unsigned int width,
        unsigned int height,
        DXGI_FORMAT Format,
        unsigned int BindFlags,
        unsigned int sampleCount = 1,
        unsigned int qualityLevels = 0);

    /**
     * @brief Inicializa la textura envolviendo un recurso existente.
     *
     * Útil para crear vistas sobre el Back Buffer o texturas de profundidad existentes.
     *
     * @param device     Referencia al dispositivo.
     * @param textureRef Textura de referencia (origen).
     * @param format     Formato para la vista.
     * @return HRESULT   S_OK si la inicialización fue exitosa.
     */
    HRESULT init(Device& device, Texture& textureRef, DXGI_FORMAT format);

    /**
     * @brief Actualiza la lógica de la textura (placeholder).
     */
    void update();

    /**
     * @brief Vincula la textura al pipeline gráfico (Pixel Shader).
     *
     * Asigna la Shader Resource View (SRV) a una ranura específica para que el shader pueda leerla.
     *
     * @param deviceContext Contexto del dispositivo.
     * @param StartSlot     Ranura inicial (t0, t1, etc.).
     * @param NumViews      Número de vistas a vincular (usualmente 1).
     */
    void render(DeviceContext& deviceContext, unsigned int StartSlot, unsigned int NumViews);

    /**
     * @brief Libera los recursos (Texture2D y ShaderResourceView).
     */
    void destroy();

public:
    /**
     * @brief Puntero al recurso crudo de la textura en GPU.
     */
    ID3D11Texture2D* m_texture = nullptr;

    /**
     * @brief Vista que permite a los shaders leer esta textura (SRV).
     * @note El nombre m_textureFromImg se mantiene por compatibilidad, pero representa el SRV.
     */
    ID3D11ShaderResourceView* m_textureFromImg = nullptr;

    /**
     * @brief Nombre o ruta del archivo para depuración.
     */
    std::string m_textureName;
};