/**
 * @file RenderPipeline.h
 * @brief Declara el orquestador de renderers de escena del motor.
 * @ingroup rendering
 */
#pragma once
#include "Rendering/ForwardRenderer.h"
#include "Rendering/DeferredRenderer.h"

 /**
  * @class RenderPipeline
  * @brief Selecciona, administra y ejecuta el renderer activo para el frame actual.
  * @details Actúa como una fachada (Facade) para los sistemas de renderizado de PorygonEngine.
  * En lugar de que el motor interactúe directamente con una implementación específica,
  * se comunica con `RenderPipeline`. Esta clase permite el intercambio en tiempo de ejecución
  * (Hot-Swapping) entre arquitecturas (Forward vs Deferred) y utiliza inicialización
  * diferida (Lazy Initialization) para instanciar los renderers en la GPU solo cuando son solicitados.
  */
class RenderPipeline {
public:
	// =========================================================================
	// CICLO DE VIDA Y GESTIÓN PRINCIPAL
	// =========================================================================

	/**
	 * @brief Inicializa el orquestador y prepara el renderer predeterminado.
	 * @param device Referencia al dispositivo de DirectX 11.
	 * @param initialRenderer El tipo de pipeline que se activará por defecto al arrancar.
	 * @return S_OK si la inicialización fue exitosa, o un código HRESULT en caso de fallo.
	 */
	HRESULT init(Device& device, RendererType initialRenderer = RendererType::Deferred);

	/**
	 * @brief Cambia la arquitectura de renderizado activa en tiempo de ejecución.
	 * @details Si el renderer solicitado no ha sido inicializado previamente, se asignarán
	 * sus recursos en la GPU en este momento.
	 * @param rendererType El nuevo tipo de renderer a activar (ej. Forward o Deferred).
	 * @param device Referencia al dispositivo para posible inicialización diferida.
	 * @return S_OK si el cambio fue exitoso, o HRESULT en caso de error.
	 */
	HRESULT setRendererType(RendererType rendererType, Device& device);

	/**
	 * @brief Redimensiona los recursos dependientes de la pantalla del renderer activo.
	 * @param device Referencia al dispositivo de DirectX 11.
	 * @param width Nuevo ancho en píxeles.
	 * @param height Nuevo alto en píxeles.
	 */
	void resize(Device& device, unsigned int width, unsigned int height);

	/**
	 * @brief Delega la ejecución del pipeline gráfico al renderer actualmente activo.
	 * @param deviceContext Contexto del dispositivo para emitir comandos de dibujado.
	 * @param camera Cámara principal que define la perspectiva de la escena.
	 * @param scene Escena lógica con los objetos y luces a procesar.
	 * @param viewportPass Render target final (usualmente el panel del editor).
	 */
	void render(DeviceContext& deviceContext,
		const Camera& camera,
		RenderScene& scene,
		EditorViewportPass& viewportPass);

	/**
	 * @brief Libera los recursos de todos los renderers inicializados y limpia la memoria.
	 */
	void destroy();

	// =========================================================================
	// INTERFAZ DE CONSULTA Y DEPURACIÓN
	// =========================================================================

	/**
	 * @brief Retorna el identificador del pipeline gráfico actualmente activo.
	 * @return Valor del enumerador RendererType.
	 */
	RendererType getRendererType() const { return m_activeRendererType; }

	/**
	 * @brief Obtiene el nombre en formato de cadena del renderer activo.
	 * @return Puntero constante a la cadena (ej. "ForwardRenderer").
	 */
	const char* getActiveRendererName() const;

	/** @brief Delega la solicitud del Shader Resource View del mapa de sombras al renderer activo. */
	ID3D11ShaderResourceView* getShadowMapSRV() const;

	/** @brief Delega la solicitud del SRV del pase previo de sombras al renderer activo. */
	ID3D11ShaderResourceView* getPreShadowSRV() const;

	// Los siguientes métodos delegarán al renderer activo. Si el ForwardRenderer está activo, 
	// devolverán nullptr de manera segura gracias al polimorfismo de ISceneRenderer.

	/** @brief Obtiene el SRV del canal Albedo/Metálico (Exclusivo de Deferred). */
	ID3D11ShaderResourceView* getGBufferAlbedoMetallicSRV() const;

	/** @brief Obtiene el SRV del canal Normal/Rugosidad (Exclusivo de Deferred). */
	ID3D11ShaderResourceView* getGBufferNormalRoughnessSRV() const;

	/** @brief Obtiene el SRV del canal Posición/AO (Exclusivo de Deferred). */
	ID3D11ShaderResourceView* getGBufferWorldAoSRV() const;

	/** @brief Obtiene el SRV del canal Emisivo/Alpha (Exclusivo de Deferred). */
	ID3D11ShaderResourceView* getGBufferEmissiveAlphaSRV() const;

	/**
	 * @brief Pasa la bandera de depuración visual de sombras al renderer activo.
	 * @param enabled true para visualizar los factores matemáticos de la sombra, false para modo normal.
	 */
	void setShadowFactorDebugEnabled(bool enabled);

private:
	// =========================================================================
	// MÉTODOS INTERNOS
	// =========================================================================

	/**
	 * @brief Garantiza que un renderer esté instanciado en memoria antes de usarlo.
	 * @details Implementa el patrón de "Lazy Initialization". Solo reserva recursos de VRAM
	 * cuando el usuario o el sistema solicita explícitamente el uso de dicho pipeline.
	 * @param rendererType El tipo de renderer a verificar/inicializar.
	 * @param device Referencia al dispositivo.
	 * @return S_OK si ya estaba inicializado o si se inicializó correctamente.
	 */
	HRESULT ensureRendererInitialized(RendererType rendererType, Device& device);

	/**
	 * @brief Resuelve y retorna un puntero al objeto renderer especificado.
	 * @param rendererType Tipo de renderer a resolver.
	 * @return Puntero a la interfaz base ISceneRenderer.
	 */
	ISceneRenderer* resolveRenderer(RendererType rendererType);

	/**
	 * @brief Resuelve y retorna un puntero constante al objeto renderer especificado.
	 * @param rendererType Tipo de renderer a resolver.
	 * @return Puntero constante a la interfaz base ISceneRenderer.
	 */
	const ISceneRenderer* resolveRenderer(RendererType rendererType) const;

private:
	ForwardRenderer m_forwardRenderer;                    /**< Instancia estática de la arquitectura Forward. */
	DeferredRenderer m_deferredRenderer;                  /**< Instancia estática de la arquitectura Diferida. */
	ISceneRenderer* m_activeRenderer = nullptr;           /**< Puntero polimórfico al renderer en uso actual. */

	RendererType m_activeRendererType = RendererType::Deferred; /**< Rastrea qué enumerador representa al renderer actual. */

	bool m_forwardInitialized = false;                    /**< Bandera que indica si el ForwardRenderer ya reservó memoria en la GPU. */
	bool m_deferredInitialized = false;                   /**< Bandera que indica si el DeferredRenderer ya reservó memoria en la GPU. */
};