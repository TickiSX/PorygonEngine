#pragma once
#include "Prerequisites.h"

class BaseApp;

/**
 * @class Window
 * @brief Encapsula la creación y gestión de una ventana nativa de Windows (Win32).
 *
 * Esta clase se encarga de registrar la clase de ventana, crear el handle (@c HWND)
 * y gestionar las dimensiones de la superficie de dibujo. Es el componente
 * fundamental sobre el cual se inicializa el SwapChain de Direct3D.
 */
class
	Window {
public:
	/**
	 * @brief Constructor por defecto.
	 */
	Window() = default;

	/**
	 * @brief Destructor por defecto.
	 */
	~Window() = default;

	/**
	 * @brief Inicializa y muestra la ventana del sistema operativo.
	 *
	 * @param hInstance  Instancia de la aplicación proporcionada por el SO.
	 * @param nCmdShow   Estado de visualización inicial de la ventana.
	 * @param wndproc    Puntero a la función de procedimiento de ventana (WndProc).
	 * @param app        Puntero a la aplicación base para vinculación de eventos.
	 * @return @c S_OK si la ventana se creó y mostró correctamente.
	 */
	HRESULT
		init(HINSTANCE hInstance, int nCmdShow, WNDPROC wndproc, BaseApp* app);

	/**
	 * @brief Procesa los eventos de la ventana en cada frame.
	 */
	void
		update();

	/**
	 * @brief Realiza operaciones de renderizado vinculadas a la ventana (si aplica).
	 */
	void
		render();

	/**
	 * @brief Destruye la ventana y libera el handle.
	 */
	void
		destroy();

public:
	/** @brief Identificador nativo de la ventana (Win32 HWND). */
	HWND m_hWnd = nullptr;

	/** @brief Ancho actual del área cliente de la ventana. */
	unsigned int m_width = 0;

	/** @brief Alto actual del área cliente de la ventana. */
	unsigned int m_height = 0;

private:
	/** @brief Instancia de la aplicación de Windows. */
	HINSTANCE m_hInst = nullptr;

	/** @brief Estructura que define los límites rectangulares de la ventana. */
	RECT m_rect;

	/** @brief Nombre que aparecerá en la barra de título de la ventana. */
	std::string m_windowName = "Wildvine Engine";
};