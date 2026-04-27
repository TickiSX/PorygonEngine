/**
 * @file ISceneRenderer.h
 * @brief Declara una interfaz común y polimórfica para los renderers de escena.
 * @ingroup rendering
 */
#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;
class Camera;
class RenderScene;
class EditorViewportPass;

/**
 * @enum RendererType
 * @brief Define los identificadores para los diferentes tipos de pipelines de renderizado soportados por el motor.
 */
enum class RendererType {
	Forward = 0,    /**< Pipeline tradicional (Forward Rendering). Evalúa la iluminación durante el pase de geometría. */
	Deferred        /**< Pipeline diferido (Deferred Shading). Separa la geometría (GBuffer) del cálculo de iluminación. */
};

/**
 * @class ISceneRenderer
 * @brief Contrato base para cualquier renderer consumido por el pipeline principal.
 * @details Proporciona una interfaz polimórfica que permite a la aplicación central (o editor)
 * intercambiar libremente entre diferentes arquitecturas de renderizado (ej. ForwardRenderer o DeferredRenderer)
 * sin modificar el código de alto nivel. Define el ciclo de vida estándar y provee métodos de acceso
 * virtuales para la visualización de texturas de depuración en la interfaz gráfica (ImGui).
 */
class ISceneRenderer {
public:
	/**
	 * @brief Destructor virtual por defecto. Garantiza la destrucción correcta de las clases derivadas.
	 */
	virtual ~ISceneRenderer() = default;

	/**
	 * @brief Inicializa los recursos base del pipeline (Buffers, Shaders, Render Targets).
	 * @param device Referencia al dispositivo de creación de recursos de DirectX 11.
	 * @return S_OK si la inicialización fue exitosa, o un código de error HRESULT.
	 */
	virtual HRESULT init(Device& device) = 0;

	/**
	 * @brief Redimensiona los recursos dependientes de la resolución de pantalla (ej. GBuffer, Viewport).
	 * @param device Referencia al dispositivo de DirectX 11.
	 * @param width Nuevo ancho de la resolución.
	 * @param height Nuevo alto de la resolución.
	 */
	virtual void resize(Device& device, unsigned int width, unsigned int height) = 0;

	/**
	 * @brief Ejecuta la secuencia completa de renderizado para el frame actual.
	 * @param deviceContext Contexto del dispositivo para emitir comandos de dibujo.
	 * @param camera Cámara principal que define la perspectiva y vista de la escena.
	 * @param scene Contenedor lógico de la escena con los objetos y luces activos.
	 * @param viewportPass Render target final destinado a ser consumido por el editor o la ventana principal.
	 */
	virtual void render(DeviceContext& deviceContext,
		const Camera& camera,
		RenderScene& scene,
		EditorViewportPass& viewportPass) = 0;

	/**
	 * @brief Libera toda la memoria y los recursos COM de DirectX asociados al renderer.
	 */
	virtual void destroy() = 0;

	// =========================================================================
	// INTERFAZ DE DEPURACIÓN (Shader Resource Views)
	// =========================================================================
	// Estos métodos poseen una implementación por defecto devolviendo nullptr. 
	// De esta forma, las clases derivadas (como ForwardRenderer) no están obligadas 
	// a implementar accesos a recursos exclusivos de arquitecturas diferidas (GBuffer).

	/** @brief Obtiene el SRV del mapa de sombras final. */
	virtual ID3D11ShaderResourceView* getShadowMapSRV() const { return nullptr; }

	/** @brief Obtiene el SRV de la escena dibujada desde la perspectiva de la luz. */
	virtual ID3D11ShaderResourceView* getPreShadowSRV() const { return nullptr; }

	/** @brief Obtiene el SRV del canal Albedo/Metallic del GBuffer (Exclusivo de Deferred). */
	virtual ID3D11ShaderResourceView* getGBufferAlbedoMetallicSRV() const { return nullptr; }

	/** @brief Obtiene el SRV del canal Normal/Roughness del GBuffer (Exclusivo de Deferred). */
	virtual ID3D11ShaderResourceView* getGBufferNormalRoughnessSRV() const { return nullptr; }

	/** @brief Obtiene el SRV del canal Posición Mundial/Ambient Occlusion del GBuffer (Exclusivo de Deferred). */
	virtual ID3D11ShaderResourceView* getGBufferWorldAoSRV() const { return nullptr; }

	/** @brief Obtiene el SRV del canal Emisivo/Alpha del GBuffer (Exclusivo de Deferred). */
	virtual ID3D11ShaderResourceView* getGBufferEmissiveAlphaSRV() const { return nullptr; }

	/**
	 * @brief Activa o desactiva la visualización de depuración del factor de sombra en pantalla.
	 * @param enabled Estado lógico de la herramienta de depuración.
	 */
	virtual void setShadowFactorDebugEnabled(bool enabled) { (void)enabled; }

	/**
	 * @brief Retorna una cadena de texto con el nombre identificador del renderer activo.
	 * @return Puntero constante a una cadena de caracteres (ej. "ForwardRenderer").
	 */
	virtual const char* getDebugName() const = 0;
};