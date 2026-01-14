#pragma once

// --- Includes del Motor (Core) ---
#include "Prerequisites.h"
#include "Window.h"
#include "Device.h"
#include "DeviceContext.h"
#include "SwapChain.h"

// --- Includes del Motor (Recursos y Pipeline) ---
#include "Texture.h"
#include "RenderTargetView.h"
#include "DepthStencilView.h"
#include "Viewport.h"
#include "ShaderProgram.h"
#include "MeshComponent.h"
#include "Buffer.h"
#include "SamplerState.h"

// --- Includes de Lógica y ECS ---
#include "Model3D.h"
#include "ECS/Actor.h"

// --- CAMBIO: Referencia a la nueva clase GUI ---
#include "EngineUtilities\GUI/GUI.h"

// --- Handler de ImGui para mensajes de Windows ---
extern IMGUI_IMPL_API
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/**
 * @class BaseApp
 * @brief Clase principal que gestiona el ciclo de vida de la aplicación con soporte para GUI avanzada.
 */
class BaseApp {
public:
	BaseApp() = default;
	~BaseApp() { destroy(); }

	HRESULT awake();
	int run(HINSTANCE hInst, int nCmdShow);
	HRESULT init();
	void update(float deltaTime);
	void render();
	void destroy();

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	// ------------------------------------------------------------------------
	// Core DirectX & Window
	// ------------------------------------------------------------------------
	Window            m_window;
	Device            m_device;
	DeviceContext     m_deviceContext;
	SwapChain         m_swapChain;
	Viewport          m_viewport;

	// ------------------------------------------------------------------------
	// Vistas y Buffers de Renderizado
	// ------------------------------------------------------------------------
	Texture           m_backBuffer;
	RenderTargetView  m_renderTargetView;
	Texture           m_depthStencil;
	DepthStencilView  m_depthStencilView;

	// ------------------------------------------------------------------------
	// Shaders y Estados
	// ------------------------------------------------------------------------
	ShaderProgram     m_shaderProgram;
	SamplerState      m_samplerState;

	// ------------------------------------------------------------------------
	// Constant Buffers (Comunicación CPU -> GPU)
	// ------------------------------------------------------------------------
	Buffer            m_cbNeverChanges;
	Buffer            m_cbChangeOnResize;
	Buffer            m_cbChangesEveryFrame;

	// Estructuras de datos locales para los buffers
	CBNeverChanges      cbNeverChanges;
	CBChangeOnResize    cbChangesOnResize;
	CBChangesEveryFrame cb;

	// ------------------------------------------------------------------------
	// Recursos Específicos y Actores
	// ------------------------------------------------------------------------
	Texture           m_cyberGunAlbedo;
	// Puedes mantener las otras texturas PBR si tu init las requiere:
	Texture           m_cyberGunNormal;
	Texture           m_cyberGunMetallic;
	Texture           m_cyberGunGlossiness;

	EU::TSharedPointer<Actor> m_cyberGun;
	std::vector<EU::TSharedPointer<Actor>> m_actors;
	Model3D* m_model;

	// ------------------------------------------------------------------------
	// Matrices Globales (Necesarias para ImGuizmo)
	// ------------------------------------------------------------------------
	XMMATRIX            m_View;
	XMMATRIX            m_Projection;
	// XMMATRIX         m_World; // Opcional según tu lógica
	// XMFLOAT4         m_vMeshColor; // Opcional

	// ------------------------------------------------------------------------
	// CAMBIO: Sistema de Interfaz (GUI)
	// ------------------------------------------------------------------------
	/**
	 * @brief Objeto que gestiona la interfaz de usuario y herramientas de edición.
	 */
	GUI               m_gui;
};