#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;
class Texture;

/**
 * @class DepthStencilView
 * @brief Encapsula un @c ID3D11DepthStencilView para usar un recurso de profundidad/est�ncil en el pipeline.
 *
 * Esta clase administra la creaci�n, aplicaci�n y liberaci�n de un @c DepthStencilView en Direct3D 11,
 * necesario para vincular un buffer de profundidad/est�ncil a la etapa de Output-Merger.
 *
 * @note No administra directamente la vida de @c Texture ni de @c DeviceContext.
 */
class
    DepthStencilView {
public:
    /**
     * @brief Constructor por defecto (no crea recursos).
     */
    DepthStencilView() = default;

    /**
     * @brief Destructor por defecto.
     * @details No libera autom�ticamente; llamar a destroy() expl�citamente.
     */
    ~DepthStencilView() = default;

    /**
     * @brief Inicializa el @c ID3D11DepthStencilView a partir de una textura de profundidad.
     *
     * Crea y asocia un @c DepthStencilView con el recurso proporcionado (generalmente una
     * textura creada con @c D3D11_BIND_DEPTH_STENCIL).
     *
     * @param device        Dispositivo con el que se crear� el recurso.
     * @param depthStencil  Textura que servir� como buffer de profundidad/est�ncil.
     * @param format        Formato DXGI con el que se crear� la vista (ejemplo: @c DXGI_FORMAT_D24_UNORM_S8_UINT).
     * @return @c S_OK si la creaci�n fue exitosa; c�digo @c HRESULT en caso contrario.
     *
     * @post Si retorna @c S_OK, @c m_depthStencilView != nullptr.
     * @sa destroy()
     */
    HRESULT
        init(Device& device, Texture& depthStencil, DXGI_FORMAT format);

    HRESULT
        init(Device& device,
            Texture& depthStencil,
            DXGI_FORMAT format,
            D3D11_DSV_DIMENSION viewDimension);

    /**
     * @brief Actualiza par�metros internos si se requieren cambios en el estado.
     *
     * M�todo de marcador para actualizar la configuraci�n del recurso.
     *
     * @note Actualmente no realiza ninguna operaci�n.
     */
    void
        update() {};

    /**
     * @brief Asigna la vista de profundidad/est�ncil al pipeline de render.
     *
     * Llama a @c OMSetRenderTargets para asociar @c m_depthStencilView al @c DeviceContext.
     *
     * @param deviceContext Contexto de dispositivo donde se enlazar� el depth-stencil view.
     *
     * @pre @c m_depthStencilView debe haberse creado con init().
     */
    void
        render(DeviceContext& deviceContext);

    /**
     * @brief Libera el recurso asociado al @c ID3D11DepthStencilView.
     *
     * Idempotente: puede llamarse m�ltiples veces de forma segura.
     *
     * @post @c m_depthStencilView == nullptr.
     */
    void
        destroy();

public:
    /**
     * @brief Vista de profundidad/est�ncil de Direct3D 11.
     * @details V�lido despu�s de una llamada exitosa a init(); @c nullptr tras destroy().
     */
    ID3D11DepthStencilView* m_depthStencilView = nullptr;
};
