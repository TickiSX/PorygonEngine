#pragma once
#include "Prerequisites.h"
#include "MeshComponent.h"

class Device;
class DeviceContext;

/**
 * @class Buffer
 * @brief Encapsula un @c ID3D11Buffer para vértices, índices o constantes.
 *
 * Esta clase administra la vida de un único buffer de D3D11 y su uso en la etapa
 * de renderizado. Soporta la creación como Vertex/Index buffer a partir de un
 * @c MeshComponent, o como Constant Buffer a partir de un tamaño específico.
 *
 * @note La instancia gestiona un solo @c ID3D11Buffer a la vez.
 * @warning No copia el recurso; manejar correctamente las referencias COM si se extiende.
 */
class
    Buffer {
public:
    /**
     * @brief Constructor por defecto (no crea recursos).
     */
    Buffer() = default;

    /**
     * @brief Destructor por defecto.
     * @note No libera automáticamente; llamar a @c destroy() para liberar el recurso COM.
     */
    ~Buffer() = default;

    /**
     * @brief Inicializa el buffer como Vertex o Index Buffer usando un @c MeshComponent.
     *
     * @param device   Dispositivo con el que se creará el recurso.
     * @param mesh     Fuente de datos (vértices/índices) para poblar el buffer.
     * @param bindFlag Bandera de enlace (@c D3D11_BIND_VERTEX_BUFFER o @c D3D11_BIND_INDEX_BUFFER).
     * @return @c S_OK si la creación fue exitosa; código @c HRESULT en caso contrario.
     */
    HRESULT
        init(Device& device, const MeshComponent& mesh, unsigned int bindFlag);

    /**
     * @brief Inicializa el buffer como Constant Buffer.
     *
     * @param device    Dispositivo con el que se creará el recurso.
     * @param ByteWidth Tamaño del buffer en bytes (debe ser múltiplo de 16).
     * @return @c S_OK si la creación fue exitosa; código @c HRESULT en caso contrario.
     */
    HRESULT
        init(Device& device, unsigned int ByteWidth);

    /**
     * @brief Actualiza el contenido del buffer (típicamente mediante @c UpdateSubresource).
     *
     * @param deviceContext  Contexto donde se realizará la actualización.
     * @param pDstResource   Recurso destino (típicamente @c m_buffer).
     * @param DstSubresource Índice de subrecurso destino (normalmente 0).
     * @param pDstBox        Región destino (@c nullptr para sobrescribir completo).
     * @param pSrcData       Puntero a los datos de origen en CPU.
     * @param SrcRowPitch    Paso por fila (se ignora en buffers).
     * @param SrcDepthPitch  Paso por profundidad (se ignora en buffers).
     */
    void
        update(DeviceContext& deviceContext,
            ID3D11Resource* pDstResource,
            unsigned int    DstSubresource,
            const D3D11_BOX* pDstBox,
            const void* pSrcData,
            unsigned int    SrcRowPitch,
            unsigned int    SrcDepthPitch);

    /**
     * @brief Enlaza el buffer a la etapa correspondiente del pipeline.
     *
     * @param deviceContext  Contexto donde se enlazará el buffer.
     * @param StartSlot      Primer slot de enlace (IA o VS/PS según tipo).
     * @param NumBuffers     Número de buffers a enlazar (típicamente 1).
     * @param setPixelShader Si es @c true y es un CB, también se enlaza al Pixel Shader.
     * @param format         Formato del índice cuando es un Index Buffer.
     */
    void
        render(DeviceContext& deviceContext,
            unsigned int   StartSlot,
            unsigned int   NumBuffers,
            bool           setPixelShader = false,
            DXGI_FORMAT    format = DXGI_FORMAT_UNKNOWN);

    /**
     * @brief Libera el @c ID3D11Buffer y resetea los metadatos internos.
     */
    void
        destroy();

    /**
     * @brief Crea un buffer genérico con una descripción y datos iniciales.
     *
     * @param device   Dispositivo con el que se creará el recurso.
     * @param desc     Descriptor del buffer (@c D3D11_BUFFER_DESC).
     * @param initData Datos iniciales (opcional).
     * @return @c S_OK si la creación fue exitosa.
     */
    HRESULT
        createBuffer(Device& device,
            D3D11_BUFFER_DESC& desc,
            D3D11_SUBRESOURCE_DATA* initData);

public:
    /** @brief Recurso COM de D3D11 administrado por la clase. */
    ID3D11Buffer* m_buffer = nullptr;

private:
    /** @brief Tamaño de un elemento en bytes (para Vertex Buffer). */
    unsigned int m_stride = 0;

    /** @brief Desplazamiento inicial en bytes (para Vertex Buffer). */
    unsigned int m_offset = 0;

    /** @brief Bandera de enlace (@c D3D11_BIND_*) que define el rol del buffer. */
    unsigned int m_bindFlag = 0;
};