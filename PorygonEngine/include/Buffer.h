#pragma once

#include "Prerequisites.h"
#include "MeshComponent.h"

// Forward Declarations
class Device;
class DeviceContext;

/**
 * @class Buffer
 * @brief Envoltorio para la gestión de buffers de DirectX 11 (ID3D11Buffer).
 *
 * Esta clase maneja la creación, actualización y vinculación (binding) de diferentes
 * tipos de buffers, tales como:
 * - Vertex Buffers (Vértices)
 * - Index Buffers (Índices)
 * - Constant Buffers (Datos uniformes para shaders)
 */
class Buffer {
public:
    /**
     * @brief Constructor por defecto.
     */
    Buffer() = default;

    /**
     * @brief Destructor por defecto.
     */
    ~Buffer() = default;

    /**
     * @brief Inicializa un buffer basado en datos de una malla (MeshComponent).
     *
     * Generalmente utilizado para crear Vertex Buffers o Index Buffers a partir de geometría existente.
     *
     * @param device    Referencia al dispositivo para la creación del recurso.
     * @param mesh      Componente de malla que contiene los datos (vértices/índices).
     * @param bindFlag  Bandera que indica el tipo de buffer (ej. D3D11_BIND_VERTEX_BUFFER).
     * @return S_OK si la creación fue exitosa.
     */
    HRESULT init(Device& device, const MeshComponent& mesh, unsigned int bindFlag);

    /**
     * @brief Inicializa un buffer vacío de un tamaño específico (generalmente Constant Buffer).
     *
     * @param device    Referencia al dispositivo.
     * @param ByteWidth Tamaño en bytes del buffer (debe ser múltiplo de 16 para Constant Buffers).
     * @return S_OK si la creación fue exitosa.
     */
    HRESULT init(Device& device, unsigned int ByteWidth);

    /**
     * @brief Actualiza los datos del buffer en la GPU.
     *
     * Envía datos desde la memoria del sistema a la subrecurso de la GPU.
     *
     * @param deviceContext Contexto del dispositivo.
     * @param pDstResource  Recurso de destino (puntero al ID3D11Resource del buffer).
     * @param DstSubresource Índice del subrecurso.
     * @param pDstBox       Caja que define la porción a actualizar (nullptr para todo).
     * @param pSrcData      Puntero a los datos en memoria CPU.
     * @param SrcRowPitch   Paso de fila (para texturas, ignorar en buffers simples).
     * @param SrcDepthPitch Paso de profundidad (para texturas 3D, ignorar en buffers simples).
     */
    void update(DeviceContext& deviceContext,
        ID3D11Resource* pDstResource,
        unsigned int DstSubresource,
        const D3D11_BOX* pDstBox,
        const void* pSrcData,
        unsigned int SrcRowPitch,
        unsigned int SrcDepthPitch);

    /**
     * @brief Vincula (Binds) el buffer al pipeline de renderizado.
     *
     * Dependiendo del `m_bindFlag` interno, esta función llamará a IASetVertexBuffers,
     * IASetIndexBuffer o VSSetConstantBuffers/PSSetConstantBuffers.
     *
     * @param deviceContext  Contexto del dispositivo.
     * @param StartSlot      Ranura (slot) de inicio para la vinculación.
     * @param NumBuffers     Número de buffers a vincular (generalmente 1).
     * @param setPixelShader Si es true y es un CB, se vincula también al Pixel Shader.
     * @param format         Formato de los datos (necesario solo para Index Buffers, ej. DXGI_FORMAT_R32_UINT).
     */
    void render(DeviceContext& deviceContext,
        unsigned int StartSlot,
        unsigned int NumBuffers,
        bool setPixelShader = false,
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);

    /**
     * @brief Libera el recurso ID3D11Buffer y limpia la memoria.
     */
    void destroy();

    /**
     * @brief Helper interno para crear el buffer crudo de DirectX.
     *
     * @param device   Dispositivo DirectX.
     * @param desc     Descriptor de la configuración del buffer.
     * @param initData Datos iniciales (opcional).
     * @return S_OK si se creó correctamente.
     */
    HRESULT createBuffer(Device& device,
        D3D11_BUFFER_DESC& desc,
        D3D11_SUBRESOURCE_DATA* initData);

private:
    ID3D11Buffer* m_buffer = nullptr; ///< Puntero al recurso COM de DirectX.
    unsigned int  m_stride = 0;       ///< Tamaño de un elemento individual (stride).
    unsigned int  m_offset = 0;       ///< Desplazamiento desde el inicio del buffer.
    unsigned int  m_bindFlag = 0;       ///< Tipo de vinculación (Vertex, Index, Constant).
};