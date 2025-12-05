#pragma once

#include "Prerequisites.h"

// Forward Declarations
class Device;
class DeviceContext;
class Window;
class Texture;

/**
 * @class SwapChain
 * @brief Encapsula la cadena de intercambio (Swap Chain) de DirectX 11.
 *
 * El Swap Chain es el mecanismo que conecta la memoria de la GPU con la ventana del sistema operativo.
 * Implementa la técnica de "Double Buffering" (Doble Búfer):
 * 1. **Back Buffer:** La GPU dibuja aquí (invisible para el usuario).
 * 2. **Front Buffer:** Lo que el monitor está mostrando actualmente.
 *
 * Cuando la GPU termina de dibujar, el Swap Chain "intercambia" (Swap/Present) los buffers,
 * mostrando el nuevo frame instantáneamente y evitando parpadeos (tearing).
 */
class SwapChain {
public:
    /**
     * @brief Constructor por defecto.
     */
    SwapChain() = default;

    /**
     * @brief Destructor por defecto.
     */
    ~SwapChain() = default;

    /**
     * @brief Inicializa el Swap Chain y configura el Back Buffer.
     *
     * Crea la cadena de intercambio asociada a la ventana (HWND) y configura
     * el muestreo múltiple (MSAA) para el suavizado de bordes (Anti-Aliasing).
     *
     * @param device        Referencia al dispositivo (para consultar calidad de MSAA).
     * @param deviceContext Contexto del dispositivo.
     * @param backBuffer    Textura que recibirá la referencia al Back Buffer creado.
     * @param window        Objeto ventana donde se presentará el contenido.
     * @return HRESULT      S_OK si la inicialización fue correcta.
     */
    HRESULT init(Device& device,
        DeviceContext& deviceContext,
        Texture& backBuffer,
        Window window);

    /**
     * @brief Actualiza la lógica del Swap Chain (placeholder).
     */
    void update();

    /**
     * @brief Operaciones de renderizado previas a la presentación.
     */
    void render();

    /**
     * @brief Libera los recursos (IDXGISwapChain y punteros auxiliares).
     */
    void destroy();

    /**
     * @brief Presenta el fotograma final en la pantalla.
     *
     * Realiza el intercambio (flip) entre el Back Buffer y el Front Buffer.
     * Sincroniza el refresco con el monitor (V-Sync) si está configurado.
     */
    void present();

public:
    /**
     * @brief Interfaz principal de DirectX para controlar la cadena de intercambio.
     */
    IDXGISwapChain* m_swapChain = nullptr;

    /**
     * @brief Tipo de driver seleccionado (Hardware, Reference, WARP).
     */
    D3D_DRIVER_TYPE m_driverType = D3D_DRIVER_TYPE_NULL;

private:
    /**
     * @brief Nivel de características (Feature Level) máximo soportado por la GPU.
     */
    D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_11_0;

    // --- Configuración de Multi-Sampling (Anti-Aliasing) ---
    unsigned int m_sampleCount;    ///< Número de muestras por píxel (ej. 4x, 8x).
    unsigned int m_qualityLevels;  ///< Nivel de calidad soportado por la GPU para ese sample count.

    // --- Interfaces DXGI (DirectX Graphics Infrastructure) ---
    IDXGIDevice* m_dxgiDevice = nullptr; ///< Interfaz para acceder a la configuración del adaptador.
    IDXGIAdapter* m_dxgiAdapter = nullptr; ///< Representa la tarjeta gráfica física.
    IDXGIFactory* m_dxgiFactory = nullptr; ///< Fábrica para crear el Swap Chain y enumerar adaptadores.
};