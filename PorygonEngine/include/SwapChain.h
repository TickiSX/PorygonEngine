#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;
class Window;
class Texture;

/**
 * @class SwapChain
 * @brief Encapsula un @c IDXGISwapChain en Direct3D 11 para administrar buffers de presentaci�n.
 *
 * Un Swap Chain es responsable de la gesti�n de los buffers de renderizado que se presentan
 * en pantalla (front y back buffer).
 * Esta clase maneja su creaci�n, actualizaci�n, renderizado y presentaci�n final.
 *
 * Tambi�n soporta configuraci�n de **MSAA (Multisample Anti-Aliasing)** para suavizado de bordes.
 */
class
    SwapChain {
public:
    /**
     * @brief Constructor por defecto.
     */
    SwapChain() = default;

    /**
     * @brief Destructor por defecto.
     * @details No libera autom�ticamente los recursos COM; llamar a destroy().
     */
    ~SwapChain() = default;

    /**
     * @brief Inicializa el Swap Chain y obtiene el back buffer.
     *
     * Crea el objeto @c IDXGISwapChain asociado a una ventana espec�fica,
     * obteniendo adem�s la textura del back buffer para el renderizado.
     *
     * @param device       Dispositivo con el que se crea el recurso.
     * @param deviceContext Contexto de dispositivo asociado.
     * @param backBuffer   Textura que representar� el back buffer.
     * @param window       Ventana de la aplicaci�n donde se presentar� la imagen.
     * @return @c S_OK si fue exitoso; c�digo @c HRESULT en caso contrario.
     *
     * @post Si retorna @c S_OK, @c m_swapChain != nullptr.
     */
    HRESULT
        init(Device& device,
            DeviceContext& deviceContext,
            Texture& backBuffer,
            Window window);

    /**
     * @brief Actualiza par�metros internos del Swap Chain.
     *
     * M�todo de marcador para soportar cambios din�micos, como resize de ventana,
     * reconfiguraci�n de MSAA u otros ajustes.
     *
     * @note Actualmente no realiza ninguna operaci�n.
     */
    void
        update();

    /**
     * @brief Ejecuta operaciones de renderizado relacionadas con el Swap Chain.
     *
     * Usualmente se utilizar�a para depuraci�n o para sincronizar buffers
     * antes de la presentaci�n.
     *
     * @note Actualmente no realiza ninguna operaci�n.
     */
    void
        render();

    /**
     * @brief Libera todos los recursos asociados al Swap Chain.
     *
     * Tambi�n libera las interfaces relacionadas de DXGI (device, adapter, factory).
     *
     * @post @c m_swapChain == nullptr.
     */
    void
        destroy();

    /**
     * @brief Presenta el back buffer en pantalla.
     *
     * Llama a @c IDXGISwapChain::Present para mostrar el contenido renderizado
     * en la ventana asociada.
     *
     * @note Si se utiliza V-Sync, puede configurarse en la implementaci�n de este m�todo.
     */
    void
        present();

    HRESULT
        resizeBuffers(UINT width, UINT height);

    HRESULT
        getBackBuffer(Texture& backBuffer);

public:
    /**
     * @brief Objeto principal del Swap Chain en Direct3D 11.
     */
    IDXGISwapChain* m_swapChain = nullptr;

    /**
     * @brief Tipo de driver utilizado (hardware, referencia, software, etc.).
     */
    D3D_DRIVER_TYPE m_driverType = D3D_DRIVER_TYPE_NULL;

private:
    /**
     * @brief Nivel de caracter�sticas de Direct3D soportado por el dispositivo.
     */
    D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_11_0;

    /**
     * @brief N�mero de muestras para MSAA.
     *
     * Ejemplo: 4 = 4x MSAA (4 muestras por p�xel).
     */
    unsigned int m_sampleCount;

    /**
     * @brief Niveles de calidad soportados para la configuraci�n de MSAA.
     */
    unsigned int m_qualityLevels;

    /**
     * @brief Interfaz DXGI para el dispositivo.
     */
    IDXGIDevice* m_dxgiDevice = nullptr;

    /**
     * @brief Interfaz DXGI para el adaptador (GPU).
     */
    IDXGIAdapter* m_dxgiAdapter = nullptr;

    /**
     * @brief Interfaz DXGI para la f�brica (creaci�n de swap chains).
     */
    IDXGIFactory* m_dxgiFactory = nullptr;
};
