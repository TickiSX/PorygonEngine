#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;

/**
 * @class DepthStencilState
 * @brief Encapsula un @c ID3D11DepthStencilState y su ciclo de vida para la etapa Output-Merger.
 *
 * Administra la creación, configuración, aplicación y destrucción de un estado de
 * profundidad/esténcil en Direct3D 11. Permite activar/desactivar pruebas de profundidad y
 * funcionalidades de esténcil según se requiera.
 *
 * @note La clase no posee ni administra la vida de @c Device ni de @c DeviceContext.
 */
class
    DepthStencilState {
public:
    /**
     * @brief Constructor por defecto (no crea recursos).
     */
    DepthStencilState() = default;

    /**
     * @brief Destructor por defecto.
     * @note No libera automáticamente el recurso COM; llamar a @c destroy().
     */
    ~DepthStencilState() = default;

    /**
     * @brief Crea el objeto @c ID3D11DepthStencilState a partir de flags configurables.
     *
     * @param device      Dispositivo con el que se crea el recurso.
     * @param depthEnable Habilita o deshabilita la prueba de profundidad.
     * @param writeMask   Define si se permite la escritura en el buffer (All o Zero).
     * @param depthFunc   Función de comparación (ej: Less, Less_Equal).
     * @return @c S_OK si la creación fue exitosa; código @c HRESULT en caso contrario.
     *
     * @post Si retorna @c S_OK, @c m_depthStencilState != @c nullptr.
     */
    HRESULT
        init(Device& device,
            bool depthEnable,
            D3D11_DEPTH_WRITE_MASK writeMask,
            D3D11_COMPARISON_FUNC depthFunc);

    /**
     * @brief Actualiza parámetros internos si la implementación lo requiere.
     * @note Actualmente no realiza ninguna operación.
     */
    void
        update();

    /**
     * @brief Aplica el estado de profundidad al contexto (@c OMSetDepthStencilState).
     *
     * @param deviceContext Contexto donde se aplicará el estado.
     * @param stencilRef    Referencia de esténcil usada por las operaciones (por defecto 0).
     * @param reset         Si es @c true, desvincula el estado (asigna @c nullptr).
     *
     * @pre @c m_depthStencilState debe haber sido creado con @c init().
     */
    void
        render(DeviceContext& deviceContext, unsigned int stencilRef = 0, bool reset = false);

    /**
     * @brief Libera el recurso COM y deja la instancia en estado no inicializado.
     */
    void
        destroy();

private:
    /**
     * @brief Recurso COM de Direct3D 11 para el estado de profundidad/esténcil.
     */
    ID3D11DepthStencilState* m_depthStencilState = nullptr;
};