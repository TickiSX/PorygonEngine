#pragma once
/**
 * @file Window.h
 * @brief Encapsula una ventana Win32 b�sica para apps con Direct3D 11.
 *
 * @details
 * Provee utilidades m�nimas para crear y mostrar una ventana (Unicode),
 * adem�s de exponer el HWND y las dimensiones del �rea cliente para
 * inicializaci�n de gr�ficos.
 */

#include "Prerequisites.h"
#include <string>

 /**
  * @class Window
  * @brief Wrapper simple de una ventana Win32.
  *
  * @details
  * - Registra una clase de ventana y crea la ventana con estilo `WS_OVERLAPPEDWINDOW`.
  * - Llama internamente a `ShowWindow` y `UpdateWindow`.
  * - Calcula y guarda el tama�o del �rea cliente (ancho/alto).
  *
  * @note Esta clase **no** administra recursos de D3D11; s�lo la ventana.
  * @warning Los miembros p�blicos existen por compatibilidad con tu c�digo actual.
  */
class Window {
public:
	/**
	 * @brief Crea y muestra la ventana principal.
	 * @param hInstance Identificador de instancia del proceso (Win32).
	 * @param nCmdShow  Par�metro est�ndar para `ShowWindow` (e.g., `SW_SHOW`).
	 * @param wndproc   Procedimiento de ventana (callback) con firma `LRESULT (CALLBACK*)(HWND, UINT, WPARAM, LPARAM)`.
	 * @return `S_OK` si tuvo �xito; en caso de error, un `HRESULT` apropiado (p. ej., `HRESULT_FROM_WIN32(GetLastError())`).
	 *
	 * @post Si tiene �xito:
	 * - `m_hInst` y `m_hWnd` quedan inicializados.
	 * - `m_width` y `m_height` reflejan el tama�o del �rea cliente.
	 * - `m_rect` guarda el rect�ngulo cliente actual.
	 */
	HRESULT init(HINSTANCE hInstance, int nCmdShow, WNDPROC wndproc);

	/**
	 * @brief Actualizaci�n por cuadro (placeholder).
	 * @details Gancho para l�gica de ventana/timers/inputs si lo necesitas.
	 */
	void update();

	/**
	 * @brief Render por cuadro (placeholder).
	 * @details No realiza dibujo por s� misma; mantenlo para consistencia.
	 */
	void render();

	/**
	 * @brief Destruye/oculta la ventana y limpia su estado.
	 * @details Suele llamar a `DestroyWindow(m_hWnd)` y poner punteros a `nullptr`.
	 * @warning No destruye recursos de D3D11. Haz esa limpieza aparte.
	 */
	void destroy();

	// ------------------------------------------------------------------
	// Campos p�blicos (compatibilidad con tu c�digo existente)
	// ------------------------------------------------------------------

	/** @brief Instancia del m�dulo (Win32). */
	HINSTANCE    m_hInst = nullptr;

	/** @brief Handle de la ventana creada. */
	HWND         m_hWnd = nullptr;

	/** @brief Rect�ngulo del �rea cliente (resultado de `GetClientRect`). */
	RECT         m_rect{ 0, 0, 0, 0 };

	/** @brief Ancho del �rea cliente en p�xeles. */
	int          m_width = 0;

	/** @brief Alto del �rea cliente en p�xeles. */
	int          m_height = 0;

	/**
	 * @brief T�tulo/nombre de la ventana (Unicode).
	 * @note Puedes cambiarlo antes de llamar a `init()` para personalizar el caption.
	 */
	std::wstring m_windowName = L"PorygonEngine";
};