#pragma once

#include "Prerequisites.h"

/**
 * @class DeviceContext
 * @brief Encapsula la interfaz ID3D11DeviceContext (Contexto Inmediato).
 *
 * Mientras que la clase @c Device se encarga de *crear* recursos, el @c DeviceContext
 * se encarga de *usarlos*. Es responsable de enviar comandos de dibujo a la GPU,
 * configurar los estados del pipeline (Shaders, Blend, Depth) y manipular los buffers.
 *
 * @note El contexto inmediato no es thread-safe; debe ser accedido por un solo hilo a la vez.
 */
class DeviceContext {
public:
    /**
     * @brief Constructor por defecto.
     */
    DeviceContext() = default;

    /**
     * @brief Destructor por defecto.
     */
    ~DeviceContext() = default;

    // ------------------------------------------------------------------------
    // Ciclo de Vida
    // ------------------------------------------------------------------------

    /**
     * @brief Inicializa el contexto (generalmente obtenido desde el Device).
     */
    void init();

    /**
     * @brief Actualiza la lógica del contexto (generalmente vacío).
     */
    void update();

    /**
     * @brief Ejecuta operaciones generales de renderizado.
     */
    void render();

    /**
     * @brief Libera la referencia al ID3D11DeviceContext.
     */
    void destroy();

    // ------------------------------------------------------------------------
    // Output Merger (OM) Stage - Configuración de destino de renderizado
    // ------------------------------------------------------------------------

    /**
     * @brief Establece dónde se dibujarán los píxeles (Render Targets) y el buffer de profundidad.
     *
     * @param NumViews             Número de vistas de render target a enlazar.
     * @param ppRenderTargetViews  Array de punteros a las vistas (RTV).
     * @param pDepthStencilView    Puntero a la vista de profundidad/stencil (DSV).
     */
    void OMSetRenderTargets(unsigned int NumViews,
        ID3D11RenderTargetView* const* ppRenderTargetViews,
        ID3D11DepthStencilView* pDepthStencilView);

    /**
     * @brief Establece el estado de mezcla (Blending).
     * Controla cómo se combinan los nuevos píxeles con los que ya existen en el buffer (transparencias).
     *
     * @param pBlendState Estado de blending (nullptr para defecto).
     * @param BlendFactor Array de 4 floats con factores de mezcla globales.
     * @param SampleMask  Máscara de cobertura de muestras (0xFFFFFFFF por defecto).
     */
    void OMSetBlendState(ID3D11BlendState* pBlendState,
        const float BlendFactor[4],
        unsigned int SampleMask);

    /**
     * @brief Limpia un Render Target con un color sólido.
     *
     * @param pRenderTargetView Vista a limpiar.
     * @param ColorRGBA         Array [R, G, B, A] con el color de fondo.
     */
    void ClearRenderTargetView(ID3D11RenderTargetView* pRenderTargetView,
        const float ColorRGBA[4]);

    /**
     * @brief Limpia el buffer de profundidad y/o stencil.
     *
     * @param pDepthStencilView Vista a limpiar.
     * @param ClearFlags        Flags (D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL).
     * @param Depth             Valor para limpiar la profundidad (usualmente 1.0f).
     * @param Stencil           Valor para limpiar el stencil (usualmente 0).
     */
    void ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencilView,
        unsigned int ClearFlags,
        FLOAT Depth,
        UINT8 Stencil);

    // ------------------------------------------------------------------------
    // Rasterizer (RS) Stage - Configuración de viewport y recortes
    // ------------------------------------------------------------------------

    /**
     * @brief Define el área de la ventana donde se dibujará (Viewport).
     *
     * @param NumViewports Número de viewports.
     * @param pViewports   Array de estructuras D3D11_VIEWPORT.
     */
    void RSSetViewports(unsigned int NumViewports,
        const D3D11_VIEWPORT* pViewports);

    /**
     * @brief Establece el estado del rasterizador (Culling, Wireframe, Scissor).
     *
     * @param pRasterizerState Puntero al objeto de estado.
     */
    void RSSetState(ID3D11RasterizerState* pRasterizerState);

    // ------------------------------------------------------------------------
    // Input Assembler (IA) Stage - Configuración de geometría
    // ------------------------------------------------------------------------

    /**
     * @brief Establece el formato de los vértices (Input Layout).
     * Define cómo interpretar los datos del Vertex Buffer.
     *
     * @param pInputLayout Puntero al layout.
     */
    void IASetInputLayout(ID3D11InputLayout* pInputLayout);

    /**
     * @brief Enlaza los buffers de vértices al pipeline.
     *
     * @param StartSlot       Ranura de entrada (usualmente 0).
     * @param NumBuffers      Número de buffers a enlazar.
     * @param ppVertexBuffers Array de punteros a los buffers.
     * @param pStrides        Array de tamaños de estructura de vértice (stride).
     * @param pOffsets        Array de desplazamientos en bytes.
     */
    void IASetVertexBuffers(unsigned int StartSlot,
        unsigned int NumBuffers,
        ID3D11Buffer* const* ppVertexBuffers,
        const unsigned int* pStrides,
        const unsigned int* pOffsets);

    /**
     * @brief Enlaza el buffer de índices.
     *
     * @param pIndexBuffer Buffer de índices.
     * @param Format       Formato (DXGI_FORMAT_R16_UINT o R32_UINT).
     * @param Offset       Desplazamiento en bytes.
     */
    void IASetIndexBuffer(ID3D11Buffer* pIndexBuffer,
        DXGI_FORMAT Format,
        unsigned int Offset);

    /**
     * @brief Define cómo se interpretan los vértices (Triángulos, Líneas, Puntos).
     *
     * @param Topology Tipo de topología (ej. D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST).
     */
    void IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology);

    // ------------------------------------------------------------------------
    // Vertex Shader (VS) Stage
    // ------------------------------------------------------------------------

    /**
     * @brief Establece el Vertex Shader activo.
     *
     * @param pVertexShader     Puntero al shader compilado.
     * @param ppClassInstances  Instancias de enlace dinámico (generalmente nullptr).
     * @param NumClassInstances Número de instancias.
     */
    void VSSetShader(ID3D11VertexShader* pVertexShader,
        ID3D11ClassInstance* const* ppClassInstances,
        UINT NumClassInstances);

    /**
     * @brief Establece Constant Buffers para el Vertex Shader.
     *
     * @param StartSlot         Ranura inicial.
     * @param NumBuffers        Número de buffers.
     * @param ppConstantBuffers Array de buffers constantes.
     */
    void VSSetConstantBuffers(UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer* const* ppConstantBuffers);

    // ------------------------------------------------------------------------
    // Pixel Shader (PS) Stage
    // ------------------------------------------------------------------------

    /**
     * @brief Establece el Pixel Shader activo.
     *
     * @param pPixelShader      Puntero al shader compilado.
     * @param ppClassInstances  Instancias de enlace dinámico (generalmente nullptr).
     * @param NumClassInstances Número de instancias.
     */
    void PSSetShader(ID3D11PixelShader* pPixelShader,
        ID3D11ClassInstance* const* ppClassInstances,
        UINT NumClassInstances);

    /**
     * @brief Establece Constant Buffers para el Pixel Shader.
     *
     * @param StartSlot         Ranura inicial.
     * @param NumBuffers        Número de buffers.
     * @param ppConstantBuffers Array de buffers constantes.
     */
    void PSSetConstantBuffers(UINT StartSlot,
        UINT NumBuffers,
        ID3D11Buffer* const* ppConstantBuffers);

    /**
     * @brief Establece recursos (Texturas) para el Pixel Shader.
     *
     * @param StartSlot             Ranura inicial de textura (t0, t1, etc.).
     * @param NumViews              Número de texturas.
     * @param ppShaderResourceViews Array de vistas de recursos (SRV).
     */
    void PSSetShaderResources(UINT StartSlot,
        UINT NumViews,
        ID3D11ShaderResourceView* const* ppShaderResourceViews);

    /**
     * @brief Establece los Samplers (modos de filtrado) para el Pixel Shader.
     *
     * @param StartSlot   Ranura inicial de sampler (s0, s1, etc.).
     * @param NumSamplers Número de samplers.
     * @param ppSamplers  Array de estados de muestreo.
     */
    void PSSetSamplers(UINT StartSlot,
        UINT NumSamplers,
        ID3D11SamplerState* const* ppSamplers);

    // ------------------------------------------------------------------------
    // Comandos de Dibujo (Draw Call)
    // ------------------------------------------------------------------------

    /**
     * @brief Dibuja primitivas indexadas. Comando final para renderizar un objeto.
     *
     * @param IndexCount         Número de índices a dibujar.
     * @param StartIndexLocation Índice de inicio en el buffer.
     * @param BaseVertexLocation Valor base sumado a cada índice (offset de vértices).
     */
    void DrawIndexed(UINT IndexCount,
        UINT StartIndexLocation,
        INT BaseVertexLocation);

    // ------------------------------------------------------------------------
    // Manipulación de Recursos (Subresources)
    // ------------------------------------------------------------------------

    /**
     * @brief Actualiza datos en la GPU desde la CPU.
     * Útil para actualizar Constant Buffers o texturas dinámicas.
     *
     * @param pDstResource   Recurso de destino en GPU.
     * @param DstSubresource Índice del subrecurso (nivel de mipmap/array).
     * @param pDstBox        Región de destino (nullptr para todo el recurso).
     * @param pSrcData       Puntero a los datos en RAM.
     * @param SrcRowPitch    Tamaño de fila en bytes (para texturas).
     * @param SrcDepthPitch  Tamaño de profundidad en bytes (para texturas 3D).
     */
    void UpdateSubresource(ID3D11Resource* pDstResource,
        unsigned int DstSubresource,
        const D3D11_BOX* pDstBox,
        const void* pSrcData,
        unsigned int SrcRowPitch,
        unsigned int SrcDepthPitch);

public:
    /**
     * @brief Puntero nativo al contexto de DirectX 11.
     */
    ID3D11DeviceContext* m_deviceContext = nullptr;
};