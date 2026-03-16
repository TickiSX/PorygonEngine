#pragma once
#include "Prerequisites.h"

/**
 * @class LayoutBuilder
 * @brief Clase de utilidad para la construcción simplificada de @c D3D11_INPUT_ELEMENT_DESC.
 *
 * Facilita la creación de layouts de entrada (Input Layouts) para los shaders mediante
 * un patrón de interfaz fluida (Fluent Interface), permitiendo encadenar llamadas
 * para definir los elementos del vértice y datos de instancia.
 */
class
    LayoutBuilder {
public:
    /**
     * @brief Constructor por defecto.
     */
    LayoutBuilder() = default;

    /**
     * @brief Destructor por defecto.
     */
    ~LayoutBuilder() = default;

    /**
     * @brief Agrega un elemento de entrada al layout (por defecto por cada vértice).
     *
     * @param semantic          Nombre de la semántica (ej: "POSITION", "TEXCOORD").
     * @param format            Formato de los datos (@c DXGI_FORMAT).
     * @param semanticIndex     Índice de la semántica.
     * @param inputSlot         Slot de entrada (0-15).
     * @param alignedByteOffset Desplazamiento en bytes. Por defecto @c D3D11_APPEND_ALIGNED_ELEMENT.
     * @param slotClass         Clasificación de los datos (vértice o instancia).
     * @param instanceStepRate  Frecuencia de paso de instancia.
     * @return Referencia a @c *this para encadenamiento.
     */
    LayoutBuilder&
        Add(const char* semantic,
            DXGI_FORMAT format,
            UINT semanticIndex = 0,
            UINT inputSlot = 0,
            UINT alignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_CLASSIFICATION slotClass = D3D11_INPUT_PER_VERTEX_DATA,
            UINT instanceStepRate = 0) {
        D3D11_INPUT_ELEMENT_DESC d{};
        d.SemanticName = semantic;
        d.SemanticIndex = semanticIndex;
        d.Format = format;
        d.InputSlot = inputSlot;
        d.AlignedByteOffset = alignedByteOffset;
        d.InputSlotClass = slotClass;
        d.InstanceDataStepRate = instanceStepRate;
        m_elems.push_back(d);
        return *this;
    }

    /**
     * @brief Atajo para agregar elementos de instancing (por instancia).
     *
     * @param semantic          Nombre de la semántica.
     * @param format            Formato de los datos (@c DXGI_FORMAT).
     * @param semanticIndex     Índice de la semántica.
     * @param inputSlot         Slot de entrada. Por defecto 1 para datos de instancia.
     * @param alignedByteOffset Desplazamiento en bytes.
     * @param instanceStepRate  Frecuencia de paso (normalmente 1).
     * @return Referencia a @c *this para encadenamiento.
     */
    LayoutBuilder&
        AddInstance(const char* semantic,
            DXGI_FORMAT format,
            UINT semanticIndex = 0,
            UINT inputSlot = 1,
            UINT alignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
            UINT instanceStepRate = 1) {
        return Add(semantic, format, semanticIndex, inputSlot, alignedByteOffset,
            D3D11_INPUT_PER_INSTANCE_DATA, instanceStepRate);
    }

    /**
     * @brief Obtiene el vector de descriptores generado.
     * @return Referencia constante al vector de @c D3D11_INPUT_ELEMENT_DESC.
     */
    const std::vector<D3D11_INPUT_ELEMENT_DESC>&
        Get() const { return m_elems; }

    /**
     * @brief Obtiene el número de elementos en el layout.
     * @return Cantidad de elementos registrados.
     */
    UINT
        Count() const { return (UINT)m_elems.size(); }

private:
    /**
     * @brief Lista de descriptores de elementos de entrada.
     */
    std::vector<D3D11_INPUT_ELEMENT_DESC> m_elems;
};