/**
 * @file Material.h
 * @brief Declara la clase base Material para la definición de superficies gráficas.
 * @ingroup rendering
 */
#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

class ShaderProgram;
class RasterizerState;
class DepthStencilState;
class SamplerState;

/**
 * @class Material
 * @brief Representa la plantilla o definición base de un material gráfico en la GPU.
 * @details Esta clase almacena la configuración de bajo nivel del pipeline gráfico
 * necesaria para dibujar un tipo específico de superficie (ej. los Shaders a ejecutar
 * y los estados de la API). Funciona en conjunto con `MaterialInstance`, la cual hereda
 * este comportamiento base pero le aplica datos específicos (como colores o texturas individuales).
 */
class Material {
public:
	// =========================================================================
	// SETTERS (Configuración del pipeline)
	// =========================================================================

	/**
	 * @brief Asigna el programa de shaders (Vertex y Pixel) que utilizará este material.
	 * @param shader Puntero al ShaderProgram compilado.
	 */
	void setShader(ShaderProgram* shader) { m_shader = shader; }

	/**
	 * @brief Define cómo el rasterizador debe procesar los polígonos (ej. Culling, Wireframe).
	 * @param state Puntero al estado de rasterizado configurado.
	 */
	void setRasterizerState(RasterizerState* state) { m_rasterizerState = state; }

	/**
	 * @brief Define cómo interactúa este material con el Z-Buffer (Depth) y el Stencil.
	 * @param state Puntero al estado de profundidad y estarcido.
	 */
	void setDepthStencilState(DepthStencilState* state) { m_depthStencilState = state; }

	/**
	 * @brief Establece las reglas de muestreo para las texturas (ej. Filtro Bilineal, Anisotrópico, Wrap).
	 * @param state Puntero al estado del muestreador.
	 */
	void setSamplerState(SamplerState* state) { m_samplerState = state; }

	/**
	 * @brief Define la categoría principal de renderizado del material (Opaco, Enmascarado, Transparente).
	 * @details El dominio afecta en qué cola de renderizado (OpaqueQueue vs TransparentQueue)
	 * se colocará la malla que utilice este material.
	 * @param domain Tipo de dominio del material.
	 */
	void setDomain(MaterialDomain domain) { m_domain = domain; }

	/**
	 * @brief Establece la ecuación matemática para mezclar los colores con el fondo (Alpha Blending, Aditivo, etc.).
	 * @param blendMode Tipo de modo de mezcla gráfica.
	 */
	void setBlendMode(BlendMode blendMode) { m_blendMode = blendMode; }

	// =========================================================================
	// GETTERS (Lectura de estados para el Renderer)
	// =========================================================================

	/**
	 * @brief Obtiene el programa de shaders asociado al material.
	 * @return Puntero al ShaderProgram.
	 */
	ShaderProgram* getShader() const { return m_shader; }

	/**
	 * @brief Obtiene el estado de rasterizado del material.
	 * @return Puntero al RasterizerState.
	 */
	RasterizerState* getRasterizerState() const { return m_rasterizerState; }

	/**
	 * @brief Obtiene el estado de control del Z-Buffer del material.
	 * @return Puntero al DepthStencilState.
	 */
	DepthStencilState* getDepthStencilState() const { return m_depthStencilState; }

	/**
	 * @brief Obtiene las reglas de filtrado de texturas del material.
	 * @return Puntero al SamplerState.
	 */
	SamplerState* getSamplerState() const { return m_samplerState; }

	/**
	 * @brief Retorna la categoría de renderizado del material (Dominio).
	 * @return Enumerador MaterialDomain actual.
	 */
	MaterialDomain getDomain() const { return m_domain; }

	/**
	 * @brief Retorna el modo en que el material mezcla colores con los píxeles ya renderizados.
	 * @return Enumerador BlendMode actual.
	 */
	BlendMode getBlendMode() const { return m_blendMode; }

private:
	ShaderProgram* m_shader = nullptr;                 /**< Shaders ejecutados en la GPU para este material. */
	RasterizerState* m_rasterizerState = nullptr;      /**< Reglas de dibujado de caras (Cull Front/Back/None). */
	DepthStencilState* m_depthStencilState = nullptr;  /**< Reglas de sobreescritura del buffer de profundidad. */
	SamplerState* m_samplerState = nullptr;            /**< Reglas de escalado y lectura de los mapas de textura. */

	MaterialDomain m_domain = MaterialDomain::Opaque;  /**< Clasificación en la arquitectura gráfica (Opaco por defecto). */
	BlendMode m_blendMode = BlendMode::Opaque;         /**< Fórmula de sobreescritura de píxeles (Sin transparencia por defecto). */
};