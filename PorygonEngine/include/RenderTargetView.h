#pragma once

#include "Prerequisites.h"

// Forward Declarations
class Device;
class DeviceContext;
class Texture;
class DepthStencilView;

/**
 * @class RenderTargetView
 * @brief Encapsula un Render Target View (RTV) de DirectX 11.
 *
 * Un Render Target View es una interfaz que permite al pipeline gráfico escribir
 * resultados (píxeles) en un recurso, ya sea:
 * 1. El Back Buffer (para mostrar en pantalla).
 * 2. Una Textura 2D (para efectos de post-proceso, sombras, espejos, etc.).
 */
class RenderTargetView {
public:
    /**
     * @brief Constructor por defecto.
     */
    RenderTargetView() = default;

    /**
     * @brief Destructor por defecto.
     */
    ~RenderTargetView() = default;

    /**
     * @brief Inicializa el RTV utilizando el Back Buffer de la Swap Chain.
     *
     * Este es el método estándar para configurar el renderizado a la ventana principal.
     *
     * @param device     Referencia al dispositivo encargado de la creación.
     * @param backBuffer Textura que representa el buffer trasero de la ventana.
     * @param format     Formato de los datos (ej. DXGI_FORMAT_R8G8B8A8_UNORM).
     * @return HRESULT   S_OK si la inicialización fue correcta.
     */
    HRESULT init(Device& device, Texture& backBuffer, DXGI_FORMAT format);

    /**
     * @brief Inicializa el RTV con una textura personalizada (Render to Texture).
     *
     * Útil para crear mapas de sombras, reflejos o mini-mapas.
     *
     * @param device        Referencia al dispositivo.
     * @param inTex         Textura de destino.
     * @param viewDimension Dimensión de la vista (ej. D3D11_RTV_DIMENSION_TEXTURE2D).
     * @param format        Formato de los datos.
     * @return HRESULT      S_OK si la inicialización fue correcta.
     */
    HRESULT init(Device& device,
        Texture& inTex,
        D3D11_RTV_DIMENSION viewDimension,
        DXGI_FORMAT format);

    /**
     * @brief Actualiza la lógica del RTV (placeholder).
     */
    void update();

    /**
     * @brief Vincula este Render Target y un Depth Buffer al pipeline (Output Merger).
     *
     * Prepara el contexto para dibujar sobre esta vista. También limpia el buffer
     * con el color especificado antes de empezar.
     *
     * @param deviceContext    Contexto del dispositivo.
     * @param depthStencilView Vista de profundidad asociada (para Z-Buffering).
     * @param numViews         Número de vistas a vincular (usualmente 1).
     * @param clearColor       Array [R, G, B, A] para limpiar el fondo.
     */
    void render(DeviceContext& deviceContext,
        DepthStencilView& depthStencilView,
        unsigned int numViews,
        const float clearColor[4]);

    /**
     * @brief Vincula solo el Render Target (sin Depth Buffer).
     *
     * Útil para interfaces 2D (UI) o post-procesos que no requieren prueba de profundidad.
     *
     * @param deviceContext Contexto del dispositivo.
     * @param numViews      Número de vistas a vincular.
     */
    void render(DeviceContext& deviceContext,
        unsigned int numViews);

    /**
     * @brief Libera el recurso ID3D11RenderTargetView.
     */
    void destroy();

    /**
     * @brief Obtiene el puntero nativo de DirectX (getter).
     * @return Puntero al recurso ID3D11RenderTargetView.
     */
    ID3D11RenderTargetView* getInterface() const { return m_renderTargetView; }

private:
    /**
     * @brief Puntero al recurso COM de DirectX.
     */
    ID3D11RenderTargetView* m_renderTargetView = nullptr;
};