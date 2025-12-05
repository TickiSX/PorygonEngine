#pragma once

#include "Prerequisites.h"

// Forward Declarations
class Device;
class DeviceContext;
class Texture;

/**
 * @class DepthStencilView
 * @brief Encapsula una vista de profundidad y stencil (DSV) en DirectX 11.
 *
 * El DepthStencilView es la interfaz que permite a la etapa de Output Merger (OM)
 * del pipeline leer y escribir en la textura de profundidad. Es crucial para:
 * - Z-Buffering: Determinar qué píxeles son visibles y cuáles están ocultos tras otros objetos.
 * - Stencil Testing: Realizar operaciones de enmascarado (espejos, sombras, recortes).
 */
class DepthStencilView {
public:
    /**
     * @brief Constructor por defecto.
     */
    DepthStencilView() = default;

    /**
     * @brief Destructor por defecto.
     */
    ~DepthStencilView() = default;

    /**
     * @brief Inicializa y crea el recurso DepthStencilView en la GPU.
     *
     * @param device       Referencia al dispositivo para la creación del recurso.
     * @param depthStencil Textura que actuará como el buffer físico de profundidad.
     * @param format       Formato de datos (ej. DXGI_FORMAT_D24_UNORM_S8_UINT).
     * @return HRESULT     S_OK si la creación fue exitosa, código de error en caso contrario.
     */
    HRESULT init(Device& device, Texture& depthStencil, DXGI_FORMAT format);

    /**
     * @brief Actualiza la lógica de la vista (sin implementación actual).
     */
    void update() {};

    /**
     * @brief Vincula o limpia la vista en el contexto actual.
     * * Dependiendo de la implementación exacta en el .cpp, esto suele limpiar
     * el buffer de profundidad antes de renderizar un nuevo frame.
     *
     * @param deviceContext Contexto del dispositivo para emitir comandos.
     */
    void render(DeviceContext& deviceContext);

    /**
     * @brief Libera el recurso COM asociado (ID3D11DepthStencilView).
     */
    void destroy();

public:
    /**
     * @brief Puntero nativo de DirectX a la vista de profundidad/stencil.
     */
    ID3D11DepthStencilView* m_depthStencilView = nullptr;
};