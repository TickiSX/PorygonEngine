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

#include "ModelLoader.h"

/**
 * @class BaseApp
 * @brief Clase principal de la aplicaci�n base del motor gr�fico.
 *        Se encarga de inicializar, actualizar, renderizar y destruir
 *        todos los componentes necesarios para ejecutar una aplicaci�n DirectX.
 */
class
	BaseApp {
public:

	/**
	 * @brief Constructor de BaseApp.
	 * @param hInst Instancia de la aplicaci�n.
	 * @param nCmdShow Par�metro que indica c�mo debe mostrarse la ventana.
	 */
	BaseApp(HINSTANCE hInst, int nCmdShow);

	/**
	 * @brief Destructor de BaseApp.
	 *        Llama autom�ticamente al m�todo destroy().
	 */
	~BaseApp() { destroy(); }

	/**
	 * @brief Ejecuta el ciclo principal de la aplicaci�n.
	 * @param hInst Instancia de la aplicaci�n.
	 * @param nCmdShow Modo de visualizaci�n de la ventana.
	 * @return C�digo de salida de la aplicaci�n.
	 */
	int
		run(HINSTANCE hInst, int nCmdShow);

	/**
	 * @brief Inicializa todos los componentes principales del motor gr�fico.
	 * @return HRESULT que indica el resultado de la inicializaci�n.
	 */
	HRESULT
		init();

	/**
	 * @brief Actualiza el estado l�gico de la aplicaci�n.
	 * @param deltaTime Tiempo transcurrido desde el �ltimo frame (en segundos).
	 */
	void
		update(float deltaTime);

	/**
	 * @brief Renderiza la escena en pantalla.
	 */
	void
		render();

	/**
	 * @brief Libera todos los recursos y objetos utilizados por la aplicaci�n.
	 */
	void
		destroy();

private:

	/**
	 * @brief Funci�n de ventana (Window Procedure) que maneja los mensajes del sistema.
	 * @param hWnd Handle de la ventana.
	 * @param message Mensaje recibido.
	 * @param wParam Par�metro adicional del mensaje.
	 * @param lParam Par�metro adicional del mensaje.
	 * @return LRESULT con el resultado del procesamiento del mensaje.
	 */
	static LRESULT CALLBACK
		WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


	Window                              m_window;
	Device                              m_device;
	DeviceContext                       m_deviceContext;
	SwapChain                           m_swapChain;
	Texture                             m_backBuffer;
	RenderTargetView                    m_renderTargetView;
	Texture                             m_depthStencil;
	DepthStencilView                    m_depthStencilView;
	Viewport                            m_viewport;
	ShaderProgram                       m_shaderProgram;
	MeshComponent												m_mesh;
	Buffer															m_vertexBuffer;
	Buffer															m_indexBuffer;
	Buffer															m_cbNeverChanges;
	Buffer															m_cbChangeOnResize;
	Buffer															m_cbChangesEveryFrame;
	Texture 														m_textureCube;
	SamplerState                        m_samplerState;

	ModelLoader                         m_modelLoader;
	LoadData                            LD;

	XMMATRIX                            m_World;
	XMMATRIX                            m_View;
	XMMATRIX                            m_Projection;
	XMFLOAT4                            m_vMeshColor; // (0.7f, 0.7f, 0.7f, 1.0f);

	CBChangeOnResize cbChangesOnResize;
	CBNeverChanges cbNeverChanges;
	CBChangesEveryFrame cb;
};