/**
 * @file MaterialInstance.h
 * @brief Declara la clase MaterialInstance, contenedora de texturas y parámetros PBR.
 * @ingroup rendering
 */
#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

class Material;
class DeviceContext;
class Texture;

/**
 * @class MaterialInstance
 * @brief Representa una instancia única de un material aplicado a una malla 3D.
 * @details Mientras que la clase `Material` define el comportamiento de bajo nivel
 * (los Shaders y los Pipeline States), `MaterialInstance` define los datos visuales
 * únicos. Contiene referencias a los mapas de textura PBR (Physically Based Rendering)
 * y los parámetros ajustables (MaterialParams) como el factor metálico o la rugosidad.
 * Múltiples instancias pueden compartir el mismo `Material` base, ahorrando memoria y
 * cambios de estado en la GPU.
 */
class MaterialInstance {
public:
	// =========================================================================
	// SETTERS (Configuración de la Instancia)
	// =========================================================================

	/**
	 * @brief Asigna el material base (la plantilla de shaders y estados) a esta instancia.
	 * @param material Puntero al objeto Material padre.
	 */
	void setMaterial(Material* material) { m_material = material; }

	/**
	 * @brief Asigna el mapa de color base (Albedo).
	 * @param texture Puntero a la textura Albedo.
	 */
	void setAlbedo(Texture* texture) { m_albedo = texture; }

	/**
	 * @brief Asigna el mapa de normales (Normal Map) para simular detalles de micro-relieve.
	 * @param texture Puntero a la textura Normal.
	 */
	void setNormal(Texture* texture) { m_normal = texture; }

	/**
	 * @brief Asigna el mapa metálico, determinando qué partes de la superficie son conductoras.
	 * @param texture Puntero a la textura Metallic.
	 */
	void setMetallic(Texture* texture) { m_metallic = texture; }

	/**
	 * @brief Asigna el mapa de rugosidad (Roughness) para definir la dispersión de los reflejos.
	 * @param texture Puntero a la textura Roughness.
	 */
	void setRoughness(Texture* texture) { m_roughness = texture; }

	/**
	 * @brief Asigna el mapa de oclusión ambiental (Ambient Occlusion) para sombras de contacto estáticas.
	 * @param texture Puntero a la textura AO.
	 */
	void setAO(Texture* texture) { m_ao = texture; }

	/**
	 * @brief Asigna el mapa emisivo para áreas de la superficie que emiten luz propia.
	 * @param texture Puntero a la textura Emissive.
	 */
	void setEmissive(Texture* texture) { m_emissive = texture; }

	// =========================================================================
	// GETTERS (Lectura de recursos)
	// =========================================================================

	/** @return Puntero al material base asociado. */
	Material* getMaterial() const { return m_material; }

	/** @return Puntero a la textura de color base. */
	Texture* getAlbedo() const { return m_albedo; }

	/** @return Puntero a la textura de normales. */
	Texture* getNormal() const { return m_normal; }

	/** @return Puntero a la textura metálica. */
	Texture* getMetallic() const { return m_metallic; }

	/** @return Puntero a la textura de rugosidad. */
	Texture* getRoughness() const { return m_roughness; }

	/** @return Puntero a la textura de oclusión ambiental. */
	Texture* getAO() const { return m_ao; }

	/** @return Puntero a la textura emisiva. */
	Texture* getEmissive() const { return m_emissive; }

	// =========================================================================
	// PARÁMETROS DEL MATERIAL (Valores escalares manipulables en la UI)
	// =========================================================================

	/**
	 * @brief Obtiene una referencia modificable a los parámetros físicos de la instancia.
	 * @details Útil para actualizar variables en tiempo real desde el Editor/Inspector (ej. ajustar EmissiveStrength).
	 * @return Referencia a la estructura MaterialParams.
	 */
	MaterialParams& getParams() { return m_params; }

	/**
	 * @brief Obtiene una referencia de solo lectura a los parámetros físicos de la instancia.
	 * @return Referencia constante a la estructura MaterialParams.
	 */
	const MaterialParams& getParams() const { return m_params; }

	// =========================================================================
	// MÉTODOS DE RENDERIZADO
	// =========================================================================

	/**
	 * @brief Vincula todas las texturas asignadas a los registros de memoria de la tarjeta gráfica (SRVs).
	 * @details Esta función debe llamarse justo antes del Draw Call. Se encarga de mapear el Albedo al
	 * registro (t0), Normal a (t1), Metallic a (t2), etc., tal como lo espera el `PBRShader.hlsl`.
	 * @param deviceContext Referencia al contexto del dispositivo para emitir los comandos de bindeo.
	 */
	void bindTextures(DeviceContext& deviceContext) const;

private:
	Material* m_material = nullptr;  /**< Referencia a la plantilla gráfica subyacente (Shaders + Estados). */

	// Texturas del flujo de trabajo PBR
	Texture* m_albedo = nullptr;     /**< Mapa RGB que define el color crudo de la superficie. */
	Texture* m_normal = nullptr;     /**< Mapa RGB que altera las normales del vértice por píxel. */
	Texture* m_metallic = nullptr;   /**< Mapa en escala de grises (Blanco = Metal, Negro = Dieléctrico). */
	Texture* m_roughness = nullptr;  /**< Mapa en escala de grises (Blanco = Áspero, Negro = Pulido/Espejo). */
	Texture* m_ao = nullptr;         /**< Mapa en escala de grises que ocluye la luz indirecta. */
	Texture* m_emissive = nullptr;   /**< Mapa RGB que determina el color e intensidad de la emisión de luz. */

	MaterialParams m_params;         /**< Estructura con variables flotantes/colores que multiplican o ajustan las texturas. */
};