/**
 * @file ForwardRenderer.h
 * @brief Declara la API de ForwardRenderer dentro del subsistema Rendering.
 * @ingroup rendering
 */
#pragma once
#include "Prerequisites.h"
#include "Buffer.h"
#include "DepthStencilState.h"
#include "DepthStencilView.h"
#include "RasterizerState.h"
#include "Rendering/ISceneRenderer.h"
#include "Rendering/RenderScene.h"
#include "Rendering/RenderTypes.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "EngineUtilities/Utilities/EditorViewportPass.h"

class Device;
class DeviceContext;
class Camera;
class Material;

/**
 * @class ForwardRenderer
 * @brief Ejecuta el pipeline de renderizado tradicional (Forward Rendering) del motor.
 * * @details Esta clase es responsable de orquestar el dibujado de la escena en un solo pase principal
 * por objeto, evaluando la iluminación directamente sobre la geometría. Se encarga de construir
 * colas de renderizado separadas (opacas y transparentes) para garantizar el orden de dibujado correcto,
 * generar mapas de sombras direccionales, actualizar los Constant Buffers por cuadro y componer
 * el resultado final en el viewport del editor.
 */
class ForwardRenderer : public ISceneRenderer {
public:
	/**
	 * @brief Inicializa buffers constantes, shaders, estados de mezcla y recursos de sombras.
	 * @param device Referencia al dispositivo de DirectX 11.
	 * @return S_OK si la inicialización fue exitosa, o un código de error HRESULT en caso de fallo.
	 */
	HRESULT init(Device& device) override;

	/**
	 * @brief Reconstruye los recursos dependientes del tamaño del viewport.
	 * @param device Referencia al dispositivo de DirectX 11.
	 * @param width Nuevo ancho de resolución.
	 * @param height Nuevo alto de resolución.
	 */
	void resize(Device& device, unsigned int width, unsigned int height) override;

	/**
	 * @brief Actualiza las constantes globales utilizadas durante el frame actual (matrices de cámara y luces).
	 * @param camera Cámara principal de la escena.
	 * @param scene Escena lógica a renderizar.
	 * @param deviceContext Contexto del dispositivo para la actualización de buffers.
	 */
	void updatePerFrame(const Camera& camera, const RenderScene& scene, DeviceContext& deviceContext);

	/**
	 * @brief Renderiza la escena completa (sombras, opacos, cielo y transparentes) sobre el `EditorViewportPass`.
	 * @param deviceContext Contexto del dispositivo de DirectX 11.
	 * @param camera Cámara principal desde la cual se dibuja la escena.
	 * @param scene Colección de objetos y luces activos.
	 * @param viewportPass Render target final del editor.
	 */
	void render(DeviceContext& deviceContext,
		const Camera& camera,
		RenderScene& scene,
		EditorViewportPass& viewportPass) override;

	/**
	 * @brief Libera de forma segura todos los recursos internos COM de DirectX y vacía las colas.
	 */
	void destroy() override;

	// =========================================================================
	// INTERFAZ DE DEPURACIÓN (ISceneRenderer)
	// =========================================================================

	/** @brief Obtiene la vista de recurso (SRV) del mapa de sombras para depuración visual. */
	ID3D11ShaderResourceView* getShadowMapSRV() const override { return m_shadowDepthSRV.m_textureFromImg; }

	/** @brief Obtiene la vista de recurso (SRV) del pase previo de sombras. */
	ID3D11ShaderResourceView* getPreShadowSRV() const override { return m_preShadowDebugPass.getSRV(); }

	/** @brief Retorna el nombre identificador del renderer para herramientas de perfilado. */
	const char* getDebugName() const override { return "ForwardRenderer"; }

private:
	// =========================================================================
	// MÉTODOS INTERNOS DEL PIPELINE
	// =========================================================================

	/**
	 * @brief Evalúa la escena y separa los objetos en colas de renderizado (Opacos vs Transparentes), ordenándolos por profundidad y material.
	 */
	void buildQueues(RenderScene& scene, const Camera& camera);

	/**
	 * @brief Genera un pase de depuración previo a la aplicación de sombras.
	 */
	void renderPreShadowDebugPass(DeviceContext& deviceContext, RenderScene& scene);

	/**
	 * @brief Ejecuta el dibujado de geometría desde la perspectiva de la luz para rellenar el Depth Buffer direccional.
	 */
	void renderShadowPass(DeviceContext& deviceContext);

	/**
	 * @brief Dibuja la cola de objetos opacos evaluando el mapa de sombras y la iluminación completa.
	 */
	void renderOpaquePass(DeviceContext& deviceContext);

	/**
	 * @brief Dibuja la cola de objetos transparentes aplicando algoritmos de mezcla (Alpha, Additive, etc.) de atrás hacia adelante.
	 */
	void renderTransparentPass(DeviceContext& deviceContext);

	/**
	 * @brief Renderiza la textura de entorno cúbico al fondo de la escena garantizando que cubra los fragmentos vacíos.
	 */
	void renderSkyboxPass(DeviceContext& deviceContext, RenderScene& scene);

	/**
	 * @brief Enlaza los buffers y texturas de un objeto individual y ejecuta su Draw Call.
	 * @param object Estructura con la información de renderizado del objeto.
	 * @param passType Indica si el objeto se procesa como Opaco o Transparente.
	 */
	void renderObject(DeviceContext& deviceContext, const RenderObject& object, RenderPassType passType);

	/**
	 * @brief Enlaza un objeto individual para ser dibujado exclusivamente en el pase de profundidad (Shadow Map).
	 */
	void renderShadowObject(DeviceContext& deviceContext, const RenderObject& object);

	/**
	 * @brief Inicializa las texturas de profundidad, vistas y shaders necesarios para el mapeo de sombras.
	 */
	HRESULT createShadowResources(Device& device);

	/**
	 * @brief Calcula la matriz de Vista y Proyección ortográfica de la luz direccional dominante.
	 */
	void updateLightMatrices(const Camera& camera, const RenderScene& scene);

	/**
	 * @brief Instancia los estados de mezcla (Opaque, Alpha, Additive, Premultiplied) de la API gráfica.
	 */
	HRESULT createBlendStates(Device& device);

	/**
	 * @brief Retorna el estado de mezcla correspondiente según la configuración del material.
	 */
	ID3D11BlendState* resolveBlendState(const Material* material) const;

private:
	// =========================================================================
	// RECURSOS DEL PIPELINE Y ESTADOS
	// =========================================================================
	Buffer m_perFrameBuffer;                          /**< Constant Buffer para datos de la cámara y luz (CBPerFrame). */
	Buffer m_perObjectBuffer;                         /**< Constant Buffer para la matriz de mundo del objeto (CBPerObject). */
	Buffer m_perMaterialBuffer;                       /**< Constant Buffer para los parámetros PBR (CBPerMaterial). */

	DepthStencilState m_transparentDepthStencil;      /**< Estado de profundidad que deshabilita la escritura en Z para transparentes. */
	DepthStencilState m_shadowDepthStencil;           /**< Estado de profundidad optimizado para el pase de sombras. */

	ID3D11BlendState* m_alphaBlendState = nullptr;         /**< Estado para Alpha Blending estándar (SrcAlpha/InvSrcAlpha). */
	ID3D11BlendState* m_opaqueBlendState = nullptr;        /**< Estado que sobrescribe completamente el color de destino. */
	ID3D11BlendState* m_additiveBlendState = nullptr;      /**< Estado de mezcla aditiva (SrcAlpha/One). */
	ID3D11BlendState* m_premultipliedBlendState = nullptr; /**< Estado para texturas con Alpha premultiplicado. */
	float m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };   /**< Factor constante opcional para operaciones de mezcla. */

	// =========================================================================
	// RECURSOS DE SHADOW MAPPING
	// =========================================================================
	Texture m_shadowDepthTexture;                     /**< Textura de profundidad para almacenar el mapa de sombras. */
	Texture m_shadowDepthSRV;                         /**< Vista de recurso de shader para leer el mapa de sombras. */
	DepthStencilView m_shadowDSV;                     /**< Vista de profundidad para escribir en el mapa de sombras. */
	ShaderProgram m_shadowShader;                     /**< Shader simplificado (Solo Posición) para proyectar vértices en la luz. */
	RasterizerState m_shadowRasterizer;               /**< Estado de rasterizador específico para mitigar el Shadow Acne (Bias/Slope). */
	unsigned int m_shadowMapSize = 2048;              /**< Resolución en píxeles del cuadrante del mapa de sombras. */

	EditorViewportPass m_preShadowDebugPass;          /**< Render Target auxiliar utilizado para fines de depuración visual. */
	bool m_applyShadows = true;                       /**< Bandera global para habilitar/deshabilitar la lectura de sombras. */

	// =========================================================================
	// ESTRUCTURAS DE DATOS LOCALES (Copias en RAM antes de enviar a GPU)
	// =========================================================================
	CBPerFrame m_cbPerFrame{};                        /**< Reflejo local del buffer constante por frame. */
	CBPerObject m_cbPerObject{};                      /**< Reflejo local del buffer constante por objeto. */
	CBPerMaterial m_cbPerMaterial{};                  /**< Reflejo local del buffer constante por material. */

	// =========================================================================
	// COLAS DE RENDERIZADO
	// =========================================================================
	std::vector<const RenderObject*> m_opaqueQueue;       /**< Lista de objetos a dibujar con Depth-Write habilitado. */
	std::vector<const RenderObject*> m_transparentQueue;  /**< Lista de objetos ordenados por profundidad a dibujar con Blending. */
};