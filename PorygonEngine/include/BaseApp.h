#pragma once
#include "Prerequisites.h"
#include "Window.h"
#include "Device.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "Texture.h"
#include "RenderTargetView.h"
#include "DepthStencilView.h"
#include "Viewport.h"
#include "ShaderProgram.h"
#include "MeshComponent.h"
#include "Buffer.h"
#include "SamplerState.h"
#include "Model3D.h"
#include "ECS/Actor.h"
#include "EngineUtilities\GUI/GUI.h"
#include "SceneGraph\SceneGraph.h"
#include "EngineUtilities\Utilities\Camera.h"
#include "EngineUtilities\Utilities\Skybox.h"
#include "EngineUtilities\Utilities\LayoutBuilder.h"
#include "EngineUtilities/Utilities/EditorViewportPass.h"

extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/**
 * @class BaseApp
 * @brief Clase principal que orquesta el ciclo de vida del motor de juego.
 *
 * BaseApp inicializa todos los subsistemas críticos (D3D11, Ventana, GUI, Grafo de Escena)
 * y contiene el bucle principal de la aplicación. Gestiona la lógica de renderizado
 * tanto de la escena como del editor.
 */
class
	BaseApp {
public:
	/**
	 * @brief Constructor por defecto.
	 */
	BaseApp() = default;

	/**
	 * @brief Destructor. Garantiza la liberación de recursos.
	 */
	~BaseApp() { destroy(); }

	/**
	 * @brief Fase de pre-inicialización.
	 * @return @c S_OK si la preparación fue exitosa.
	 */
	HRESULT
		awake();

	/**
	 * @brief Punto de entrada del bucle de mensajes de Windows.
	 * @param hInst     Instancia de la aplicación.
	 * @param nCmdShow  Estado de visualización de la ventana.
	 * @return Código de salida de la aplicación.
	 */
	int
		run(HINSTANCE hInst, int nCmdShow);

	/**
	 * @brief Inicializa los recursos de Direct3D, Escena y GUI.
	 * @return @c S_OK si todo se configuró correctamente.
	 */
	HRESULT
		init();

	/**
	 * @brief Actualiza la lógica del motor en cada frame.
	 * @param deltaTime Tiempo transcurrido desde el último frame.
	 */
	void
		update(float deltaTime);

	/**
	 * @brief Ejecuta el pipeline de renderizado completo (Escena + Editor).
	 */
	void
		render();

	/**
	 * @brief Libera todos los recursos cargados y apaga los subsistemas.
	 */
	void
		destroy();

	/**
	 * @brief Callback para el redimensionamiento de la ventana principal.
	 * @param newW Nuevo ancho.
	 * @param newH Nuevo alto.
	 */
	void
		onResize(UINT newW, UINT newH);

	/**
	 * @brief Gestiona el redimensionamiento diferido del viewport del editor.
	 */
	void
		handleEditorViewportResize();

private:
	/**
	 * @brief Procedimiento de ventana para gestionar mensajes del SO.
	 */
	static LRESULT CALLBACK
		WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	// --- Infraestructura Base ---
	Window            m_window;
	Device            m_device;
	DeviceContext     m_deviceContext;
	SwapChain         m_swapChain;

	// --- Recursos de Renderizado Principales ---
	Texture           m_backBuffer;
	RenderTargetView  m_renderTargetView;
	Texture           m_depthStencil;
	DepthStencilView  m_depthStencilView;
	Viewport          m_viewport;
	ShaderProgram     m_shaderProgram;

	// --- Lógica de Aplicación ---
	bool              m_d3dReady = false;
	Buffer            m_constantBuffer;
	CBMain            m_constantBufferStruct;
	Camera            m_camera;
	SceneGraph        m_sceneGraph;
	GUI               m_gui;
	Skybox            m_skybox;

	// --- Recursos y Actores ---
	std::vector<EU::TSharedPointer<Actor>> m_actors;
	EU::TSharedPointer<Actor>              m_cyberGun;
	Model3D* m_model;
	Texture                                m_skyboxTex;

	// --- Texturas PBR ---
	Texture m_AlbedoSRV;
	Texture m_MetallicSRV;
	Texture m_RoughnessSRV;
	Texture m_AOSRV;
	Texture m_NormalSRV;

	// --- Estados de Pipeline ---
	RasterizerState   m_defaultRasterizer;
	DepthStencilState m_defaultDepthStencil;

	// --- Gestión de Viewport del Editor ---
	EditorViewportPass m_editorViewportPass;
	bool               m_editorViewportResizePending = false;
	unsigned int       m_pendingViewportWidth = 1;
	unsigned int       m_pendingViewportHeight = 1;
	unsigned int       m_lastRequestedViewportWidth = 1;
	unsigned int       m_lastRequestedViewportHeight = 1;
	int                m_viewportResizeStableFrames = 0;
};