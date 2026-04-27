/**
 * @file DeferredRenderer.h
 * @brief Declara la API de DeferredRenderer dentro del subsistema Rendering.
 * @ingroup rendering
 */
#pragma once
#include "Buffer.h"
#include "DepthStencilState.h"
#include "DepthStencilView.h"
#include "RasterizerState.h"
#include "Rendering/ISceneRenderer.h"
#include "Rendering/RenderScene.h"
#include "Rendering/RenderTypes.h"
#include "SamplerState.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "EngineUtilities/Utilities/EditorViewportPass.h"

class Device;
class DeviceContext;
class Camera;
class Material;

/**
 * @class DeferredRenderer
 * @brief Implementa un pipeline de renderizado diferido (Deferred Shading) con GBuffer y Lighting Pass.
 * * @details Este renderer utiliza una arquitectura híbrida:
 * 1. **Geometry Pass (Deferred):** Dibuja toda la geometría opaca en múltiples Render Targets (GBuffer).
 * El GBuffer actual guarda: [Albedo+Metallic], [Normal+Roughness], [WorldPos+AO], [Emissive+Alpha].
 * 2. **Lighting Pass (Deferred):** Utiliza un Full-Screen Quad para calcular la luz por píxel leyendo el GBuffer.
 * 3. **Forward Pass:** Dibuja los objetos transparentes al final del pipeline, ya que la transparencia
 * no es compatible con el pase diferido tradicional.
 */
class DeferredRenderer : public ISceneRenderer {
public:
	/**
	 * @brief Inicializa los recursos del pipeline diferido (Shaders, Buffers, GBuffer y Shadow Maps).
	 * @param device Referencia al dispositivo de DirectX 11.
	 * @return S_OK si la inicialización fue exitosa, o un código de error HRESULT en caso de fallo.
	 */
	HRESULT init(Device& device) override;

	/**
	 * @brief Reconstruye los Render Targets del GBuffer cuando cambia la resolución de la ventana.
	 * @param device Referencia al dispositivo de DirectX 11.
	 * @param width Nuevo ancho en píxeles.
	 * @param height Nuevo alto en píxeles.
	 */
	void resize(Device& device, unsigned int width, unsigned int height) override;

	/**
	 * @brief Ejecuta la secuencia completa de renderizado (Sombras -> GBuffer -> Iluminación -> Transparencias).
	 * @param deviceContext Referencia al contexto del dispositivo.
	 * @param camera Cámara activa que visualiza la escena.
	 * @param scene Escena lógica con los objetos y luces a dibujar.
	 * @param viewportPass El paso del editor al cual se enviará el resultado final.
	 */
	void render(DeviceContext& deviceContext,
		const Camera& camera,
		RenderScene& scene,
		EditorViewportPass& viewportPass) override;

	/**
	 * @brief Libera toda la memoria y recursos COM de DirectX asociados al renderer.
	 */
	void destroy() override;

	// =========================================================================
	// METODOS DE DEPURACION Y ACCESO A TEXTURAS (Usados por ImGui)
	// =========================================================================
	ID3D11ShaderResourceView* getShadowMapSRV() const override { return m_shadowDepthSRV.m_textureFromImg; }
	ID3D11ShaderResourceView* getPreShadowSRV() const override { return m_preShadowDebugPass.getSRV(); }
	ID3D11ShaderResourceView* getGBufferAlbedoMetallicSRV() const override { return m_gBufferAlbedoMetallicSRV.m_textureFromImg; }
	ID3D11ShaderResourceView* getGBufferNormalRoughnessSRV() const override { return m_gBufferNormalRoughnessSRV.m_textureFromImg; }
	ID3D11ShaderResourceView* getGBufferWorldAoSRV() const override { return m_gBufferWorldAoSRV.m_textureFromImg; }
	ID3D11ShaderResourceView* getGBufferEmissiveAlphaSRV() const override { return m_gBufferEmissiveAlphaSRV.m_textureFromImg; }

	void setShadowFactorDebugEnabled(bool enabled) override { m_shadowFactorDebugEnabled = enabled; }
	const char* getDebugName() const override { return "DeferredRenderer"; }

private:
	// =========================================================================
	// METODOS INTERNOS DEL PIPELINE
	// =========================================================================

	/** @brief Clasifica los objetos de la escena en colas (Opacos y Transparentes). */
	void buildQueues(RenderScene& scene, const Camera& camera);

	/** @brief Actualiza el Constant Buffer principal (Matrices, Cámara, Luz). */
	void updatePerFrame(const Camera& camera, const RenderScene& scene, DeviceContext& deviceContext);

	/** @brief Genera la matriz ortográfica ViewProjection para el pase de sombras direccionales. */
	void updateLightMatrices(const Camera& camera, const RenderScene& scene);

	/** @brief Orquesta el renderizado hacia el Viewport del Editor. */
	void renderSceneToTarget(DeviceContext& deviceContext, RenderScene& scene, EditorViewportPass& targetPass, bool applyShadows);

	/** @brief Vincula simultáneamente los 4 Render Targets del GBuffer al Output Merger. */
	void bindGBufferTargets(DeviceContext& deviceContext, ID3D11DepthStencilView* depthStencilView);

	/** @brief Cambia el Output Merger al objetivo de renderizado final (Pantalla/Editor). */
	void bindFinalTarget(DeviceContext& deviceContext, ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView);

	/** @brief Limpia los recursos (SRV) del GBuffer de los Shaders para evitar conflictos de lectura/escritura. */
	void clearDeferredSRVs(DeviceContext& deviceContext);

	/** @brief Primer pase: Renderiza geometría opaca para rellenar las texturas del GBuffer. */
	void renderGeometryPass(DeviceContext& deviceContext);
	void renderGeometryObject(DeviceContext& deviceContext, const RenderObject& object);

	/** @brief Segundo pase: Renderiza un Quad a pantalla completa calculando iluminación desde el GBuffer. */
	void renderLightingPass(DeviceContext& deviceContext);

	/** @brief Renderiza la caja de cielo en el fondo utilizando la prueba de profundidad estricta. */
	void renderSkyboxPass(DeviceContext& deviceContext, RenderScene& scene);

	/** @brief Tercer pase: Dibuja objetos transparentes sobre el resultado iluminado (Forward). */
	void renderTransparentPass(DeviceContext& deviceContext);
	void renderForwardObject(DeviceContext& deviceContext, const RenderObject& object, RenderPassType passType);

	/** @brief Pase previo: Renderiza la escena desde la perspectiva de la luz para generar el Depth Map. */
	void renderShadowPass(DeviceContext& deviceContext);
	void renderShadowObject(DeviceContext& deviceContext, const RenderObject& object);

	// =========================================================================
	// CREACION DE RECURSOS (Llamados durante init)
	// =========================================================================
	HRESULT createShadowResources(Device& device);
	HRESULT createGBufferResources(Device& device, unsigned int width, unsigned int height);
	HRESULT createGBufferTarget(Device& device, unsigned int width, unsigned int height, DXGI_FORMAT format, Texture& texture, Texture& srv, RenderTargetView& rtv);
	HRESULT createLightingResources(Device& device);
	HRESULT createFullScreenQuad(Device& device);
	HRESULT createBlendStates(Device& device);
	ID3D11BlendState* resolveBlendState(const Material* material) const;

private:
	// Buffers Constantes e Índices/Vértices Globales
	Buffer m_perFrameBuffer;
	Buffer m_perObjectBuffer;
	Buffer m_perMaterialBuffer;
	Buffer m_lightingDebugBuffer;
	Buffer m_fullscreenVertexBuffer;
	Buffer m_fullscreenIndexBuffer;

	// Estados del Pipeline
	DepthStencilState m_transparentDepthStencil;
	DepthStencilState m_disabledDepthStencil;     /**< Usado para dibujar el Fullscreen Quad sin escribir en el Z-Buffer. */
	DepthStencilState m_shadowDepthStencil;

	ID3D11BlendState* m_alphaBlendState = nullptr;
	ID3D11BlendState* m_opaqueBlendState = nullptr;
	ID3D11BlendState* m_additiveBlendState = nullptr;
	ID3D11BlendState* m_premultipliedBlendState = nullptr;
	float m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Recursos de Shadow Mapping
	Texture m_shadowDepthTexture;
	Texture m_shadowDepthSRV;
	DepthStencilView m_shadowDSV;
	ShaderProgram m_shadowShader;
	RasterizerState m_shadowRasterizer;
	unsigned int m_shadowMapSize = 2048;

	// Shaders y Estados Generales
	ShaderProgram m_gBufferShader;
	ShaderProgram m_deferredLightingShader;
	SamplerState m_lightingSampler;
	RasterizerState m_fullscreenRasterizer;

	// =========================================================================
	// G-BUFFER TARGETS
	// =========================================================================
	Texture m_gBufferAlbedoMetallicTexture;
	Texture m_gBufferAlbedoMetallicSRV;
	RenderTargetView m_gBufferAlbedoMetallicRTV;

	Texture m_gBufferNormalRoughnessTexture;
	Texture m_gBufferNormalRoughnessSRV;
	RenderTargetView m_gBufferNormalRoughnessRTV;

	Texture m_gBufferWorldAoTexture;
	Texture m_gBufferWorldAoSRV;
	RenderTargetView m_gBufferWorldAoRTV;

	Texture m_gBufferEmissiveAlphaTexture;
	Texture m_gBufferEmissiveAlphaSRV;
	RenderTargetView m_gBufferEmissiveAlphaRTV;

	// Utilidades de Renderizado y Depuración
	EditorViewportPass m_preShadowDebugPass;
	bool m_applyShadows = true;
	unsigned int m_renderWidth = 1280;
	unsigned int m_renderHeight = 720;

	// Datos de los Constant Buffers
	CBPerFrame m_cbPerFrame{};
	CBPerObject m_cbPerObject{};
	CBPerMaterial m_cbPerMaterial{};

	struct DeferredLightingDebugData {
		int DebugViewMode = 0;
		float ShadowStrength = 1.0f;
		float pad0 = 0.0f;
		float pad1 = 0.0f;
	} m_lightingDebugData{};

	bool m_shadowFactorDebugEnabled = false;

	// Colas de renderizado dinámicas
	std::vector<const RenderObject*> m_opaqueQueue;
	std::vector<const RenderObject*> m_transparentQueue;
};