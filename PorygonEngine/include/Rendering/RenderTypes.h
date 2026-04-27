/**
 * @file RenderTypes.h
 * @brief Define enumeraciones y estructuras de datos fundamentales para el subsistema de renderizado.
 * @ingroup rendering
 */
#pragma once
#include "Prerequisites.h"

class Mesh;
class MaterialInstance;

// =========================================================================
// ENUMERACIONES DEL PIPELINE GRÁFICO
// =========================================================================

/**
 * @enum MaterialDomain
 * @brief Clasifica los materiales según su comportamiento de opacidad para el ordenamiento de colas.
 */
enum class MaterialDomain {
	Opaque = 0,    /**< Superficies completamente sólidas. Se dibujan primero (aprovechan Early-Z). */
	Masked,        /**< Superficies sólidas con recortes por canal Alpha (ej. follaje, vallas). */
	Transparent    /**< Superficies translúcidas. Se dibujan al final, ordenadas de atrás hacia adelante. */
};

/**
 * @enum BlendMode
 * @brief Define la ecuación matemática para mezclar un píxel nuevo con el color ya existente en pantalla.
 */
enum class BlendMode {
	Opaque = 0,            /**< Sobrescribe el color de destino por completo. */
	Alpha,                 /**< Mezcla tradicional basada en la transparencia del píxel origen (SrcAlpha / InvSrcAlpha). */
	Additive,              /**< Suma los colores. Útil para luces, fuego o magia (SrcAlpha / One). */
	PremultipliedAlpha     /**< Mezcla donde el color RGB ya fue multiplicado por su Alpha previamente. */
};

/**
 * @enum RenderPassType
 * @brief Identifica la etapa actual de dibujado dentro del ciclo de vida del frame.
 */
enum class RenderPassType {
	Shadow = 0,    /**< Pase de renderizado desde la perspectiva de la luz (Depth-Only). */
	Opaque,        /**< Pase principal para la geometría sólida. */
	Skybox,        /**< Pase de renderizado del entorno de fondo. */
	Transparent,   /**< Pase de renderizado para geometría translúcida. */
	Editor         /**< Pase reservado para elementos de la interfaz, herramientas y gizmos. */
};

/**
 * @enum LightType
 * @brief Define el comportamiento volumétrico y direccional de una fuente de luz.
 */
enum class LightType {
	Directional = 0, /**< Luz paralela infinita (ej. el Sol). No tiene posición real, solo dirección. */
	Point,           /**< Luz omnidireccional que emite desde un punto central disipándose con la distancia. (WIP) */
	Spot             /**< Luz con forma de cono que emite desde un punto en una dirección específica. (WIP) */
};

// =========================================================================
// ESTRUCTURAS DE DATOS DE ALTO NIVEL (LÓGICA / CPU)
// =========================================================================

/**
 * @struct LightData
 * @brief Contiene las propiedades físicas de una fuente de iluminación.
 */
struct LightData {
	LightType type = LightType::Directional;                /**< Clasificación de la luz. */
	EU::Vector3 color = EU::Vector3(1.0f, 1.0f, 1.0f);      /**< Tinte RGB de la luz. */
	float intensity = 1.0f;                                 /**< Multiplicador de fuerza lumínica. */

	EU::Vector3 direction = EU::Vector3(0.0f, -1.0f, 0.0f); /**< Vector de dirección (Crítico para luces Directional y Spot). */
	float range = 0.0f;                                     /**< Distancia máxima de influencia (Aplica a luces Point y Spot). */

	EU::Vector3 position = EU::Vector3(0.0f, 0.0f, 0.0f);   /**< Coordenada en el mundo (Ignorada por luces Directional). */
	float spotAngle = 0.0f;                                 /**< Ángulo de apertura del cono en radianes (Exclusivo para Spot). */
};

/**
 * @struct MaterialParams
 * @brief Parámetros físicos del material ajustables desde el editor (Interfaz de usuario).
 * @details Estos valores actúan como multiplicadores o límites para las texturas PBR asignadas.
 */
struct MaterialParams {
	XMFLOAT4 baseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);  /**< Tinte base (RGBA). */
	float metallic = 1.0f;                                  /**< Multiplicador de conductividad metálica (0.0 a 1.0). */
	float roughness = 1.0f;                                 /**< Multiplicador de rugosidad micro-superficial (0.0 a 1.0). */
	float ao = 1.0f;                                        /**< Multiplicador de oclusión ambiental. */
	float normalScale = 1.0f;                               /**< Intensidad del relieve (Normal Map). */
	float emissiveStrength = 1.0f;                          /**< Multiplicador de intensidad para la luz emitida. */
	float alphaCutoff = 0.5f;                               /**< Umbral de descarte de píxeles para materiales de dominio Masked. */
};

/**
 * @struct RenderObject
 * @brief Paquete de datos que encapsula una entidad lista para ser procesada por el Renderer.
 * @details Extraído del SceneGraph en cada frame. Contiene únicamente lo necesario para emitir un Draw Call.
 */
struct RenderObject {
	Mesh* mesh = nullptr;                                   /**< Puntero a la geometría a dibujar. */
	MaterialInstance* materialInstance = nullptr;           /**< Puntero al material principal. */
	std::vector<MaterialInstance*> materialInstances;       /**< Lista de materiales en caso de tener múltiples submallas. */
	XMMATRIX world = XMMatrixIdentity();                    /**< Matriz de transformación (Posición, Rotación, Escala) en el espacio global. */
	bool castShadow = true;                                 /**< Indica si el objeto proyectará sombras. */
	bool transparent = false;                               /**< Indica si el objeto requiere Blending (lo envía a la TransparentQueue). */
	float distanceToCamera = 0.0f;                          /**< Distancia cuadrada al ojo de la cámara (Usado para ordenamiento de colas). */
};

// =========================================================================
// CONSTANT BUFFERS (HLSL MEMORY MAPPING)
// ADVERTENCIA: La alineación de bytes debe coincidir exactamente con los cbuffers del shader (múltiplos de 16 bytes).
// =========================================================================

/**
 * @struct CBPerFrame
 * @brief Estructura de memoria mapeada al registro `b0` del Vertex/Pixel Shader. (240 bytes).
 * @details Actualizado una vez por fotograma. Contiene las matrices globales y la información de la luz dominante.
 */
struct CBPerFrame {
	XMFLOAT4X4 View{};                                      /**< Matriz de la cámara (64 bytes). */
	XMFLOAT4X4 Projection{};                                /**< Matriz de perspectiva de la cámara (64 bytes). */
	XMFLOAT4X4 LightViewProjection{};                       /**< Matriz para el cálculo de coordenadas de sombras (64 bytes). */

	EU::Vector3 CameraPos{};                                /**< Posición de la cámara en el mundo (12 bytes). */
	float pad0 = 0.0f;                                      /**< Relleno para alinear a 16 bytes (4 bytes). */

	EU::Vector3 LightDir = EU::Vector3(0.0f, -1.0f, 0.0f);  /**< Dirección de la luz principal (12 bytes). */
	float pad1 = 0.0f;                                      /**< Relleno para alinear a 16 bytes (4 bytes). */

	EU::Vector3 LightColor = EU::Vector3(1.0f, 1.0f, 1.0f); /**< Color e intensidad de la luz (12 bytes). */
	float pad2 = 0.0f;                                      /**< Relleno para alinear a 16 bytes (4 bytes). */
};

/**
 * @struct CBPerObject
 * @brief Estructura de memoria mapeada al registro `b1` del Vertex Shader. (64 bytes).
 * @details Actualizado por cada Draw Call (para cada objeto individual de la escena).
 */
struct CBPerObject {
	XMFLOAT4X4 World{};                                     /**< Matriz de transformación local a global del modelo (64 bytes). */
};

/**
 * @struct CBPerMaterial
 * @brief Estructura de memoria mapeada al registro `b2` del Pixel Shader. (48 bytes útiles + 32 bytes de relleno = 80 bytes).
 * @details Actualizado cada vez que el Renderer procesa un material distinto. Refleja los valores de `MaterialParams`.
 */
struct CBPerMaterial {
	XMFLOAT4 BaseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);  /**< Color base (16 bytes). */

	float Metallic = 1.0f;                                  /**< Propiedad metálica (4 bytes). */
	float Roughness = 1.0f;                                 /**< Propiedad de rugosidad (4 bytes). */
	float AO = 1.0f;                                        /**< Intensidad de Oclusión Ambiental (4 bytes). */
	float NormalScale = 1.0f;                               /**< Escala del mapa de normales (4 bytes). */

	float EmissiveStrength = 1.0f;                          /**< Intensidad emisiva (4 bytes). */
	float AlphaCutoff = 0.0f;                               /**< Umbral Alpha (4 bytes). */

	// Relleno explícito para empaquetado de memoria en GPU (Asegurando alineación a bloques de 16 bytes o float4)
	float pad0 = 0.0f;
	float pad1 = 0.0f;
	float pad2 = 0.0f;
	float pad3 = 0.0f;
	float pad4 = 0.0f;
	float pad5 = 0.0f;
};