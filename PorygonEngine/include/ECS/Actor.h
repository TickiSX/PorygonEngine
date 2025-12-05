#pragma once

#include "Prerequisites.h"
#include "Entity.h"
#include "Buffer.h"
#include "Texture.h"
#include "Transform.h"
#include "SamplerState.h"
#include "ShaderProgram.h"

// Forward declarations
class Device;
class DeviceContext;
class MeshComponent;
// #include "Rasterizer.h"
// #include "BlendState.h"
// #include "DepthStencilState.h"

/**
 * @class Actor
 * @brief Representa una entidad gráfica en la escena con capacidades de renderizado.
 *
 * Un Actor es una entidad del motor que agrupa mallas, texturas y los recursos necesarios
 * para dibujar un objeto 3D. Gestiona buffers de vértices e índices, estados de rasterización
 * y shaders, además de soportar el pase de renderizado de sombras.
 */
class Actor : public Entity {
public:
    /**
     * @brief Constructor por defecto.
     */
    Actor() = default;

    /**
     * @brief Constructor que inicializa el actor con un dispositivo.
     * @param device Referencia al dispositivo usado para inicializar recursos.
     */
    Actor(Device& device);

    /**
     * @brief Destructor virtual.
     */
    virtual ~Actor() = default;

    /**
     * @brief Inicializa el actor.
     * @note Método heredado de Entity.
     */
    void init() override {}

    /**
     * @brief Actualiza la lógica del actor en cada frame.
     *
     * @param deltaTime     Tiempo transcurrido desde la última actualización.
     * @param deviceContext Contexto del dispositivo para operaciones gráficas.
     */
    void update(float deltaTime, DeviceContext& deviceContext) override;

    /**
     * @brief Renderiza el actor en la escena.
     *
     * Configura estados de render, buffers y shaders antes de emitir los comandos de dibujo.
     *
     * @param deviceContext Contexto del dispositivo para operaciones gráficas.
     */
    void render(DeviceContext& deviceContext) override;

    /**
     * @brief Libera manualmente todos los recursos asociados al actor.
     *
     * Destruye buffers, estados, shaders y libera referencias a texturas.
     */
    void destroy();

    /**
     * @brief Establece y configura las mallas del actor.
     *
     * Inicializa los buffers de vértices e índices en la GPU para las mallas proporcionadas.
     *
     * @param device Dispositivo con el cual se crearán los buffers.
     * @param meshes Vector de componentes de malla a asignar.
     */
    void setMesh(Device& device, std::vector<MeshComponent> meshes);

    /**
     * @brief Obtiene el nombre del actor.
     * @return String con el nombre actual.
     */
    std::string getName() { return m_name; }

    /**
     * @brief Establece el nombre del actor.
     * @param name Nuevo nombre para identificar al actor.
     */
    void setName(const std::string& name) { m_name = name; }

    /**
     * @brief Asigna las texturas al actor.
     * @param textures Vector de texturas a utilizar.
     */
    void setTextures(std::vector<Texture> textures) { m_textures = textures; }

    /**
     * @brief Define si el actor debe proyectar sombras.
     * @param v 'true' para habilitar proyección de sombras, 'false' para deshabilitar.
     */
    void setCastShadow(bool v) { castShadow = v; }

    /**
     * @brief Consulta si el actor está configurado para proyectar sombras.
     * @return 'true' si proyecta sombras, 'false' en caso contrario.
     */
    bool canCastShadow() const { return castShadow; }

    /**
     * @brief Renderiza el actor para el mapa de sombras (Shadow Map).
     *
     * Utiliza un shader simplificado para proyectar la profundidad desde la luz.
     *
     * @param deviceContext Contexto del dispositivo.
     */
    void renderShadow(DeviceContext& deviceContext);

private:
    // --- Recursos Gráficos ---
    std::vector<MeshComponent> m_meshes;        ///< Lista de mallas que componen el actor.
    std::vector<Texture>       m_textures;      ///< Texturas aplicadas a las mallas.
    std::vector<Buffer>        m_vertexBuffers; ///< Buffers de vértices en GPU.
    std::vector<Buffer>        m_indexBuffers;  ///< Buffers de índices en GPU.

    // --- Estados y Shaders ---
    SamplerState        m_sampler;              ///< Estado de muestreo para texturas.
    CBChangesEveryFrame m_model;                ///< Estructura de datos para el Constant Buffer (World Matrix).
    Buffer              m_modelBuffer;          ///< Constant Buffer en GPU para transformaciones.

    // --- Recursos de Sombras ---
    ShaderProgram       m_shaderShadow;         ///< Shader program para el pase de sombras.
    Buffer              m_shaderBuffer;         ///< Buffer auxiliar para datos de sombras.
    CBChangesEveryFrame m_cbShadow;             ///< Constant buffer específico para sombras.
    XMFLOAT4            m_LightPos;             ///< Posición de la luz para cálculo de sombras.

    // --- Propiedades Generales ---
    std::string m_name = "Actor";               ///< Nombre identificador.
    bool        castShadow = true;              ///< Bandera de control de sombras.

    // --- Recursos Reservados/Comentados ---
    // BlendState        m_blendstate;
    // Rasterizer        m_rasterizer;
    // BlendState        m_shadowBlendState;
    // DepthStencilState m_shadowDepthStencilState;
};