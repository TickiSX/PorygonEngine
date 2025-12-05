#pragma once

#include "Prerequisites.h"

/**
 * @class Window
 * @brief Encapsula una ventana del sistema operativo Windows (Win32 API).
 *
 * Esta clase gestiona el ciclo de vida de la ventana, que incluye:
 * 1. Registro de la clase de ventana (WNDCLASSEX).
 * 2. Creación de la ventana física (CreateWindowEx).
 * 3. Gestión del bucle de mensajes (si se implementa en update).
 * 4. Definición del área cliente donde DirectX renderizará los gráficos.
 */
class Window {
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
	 * @brief Inicializa y crea la ventana de la aplicación.
	 *
	 * Registra la clase de ventana y realiza la llamada a CreateWindow.
	 *
	 * @param hInstance Manejador (Handle) de la instancia de la aplicación actual.
	 * @param nCmdShow  Comando que indica cómo se debe mostrar la ventana (minimizado, maximizado, etc.).
	 * @param wndproc   Puntero a la función de procedimiento de ventana (Callback para eventos de input).
	 * @return HRESULT  S_OK si la ventana se creó correctamente, E_FAIL si falló.
	 */
	HRESULT init(HINSTANCE hInstance, int nCmdShow, WNDPROC wndproc);

	/**
	 * @brief Actualiza la lógica de la ventana.
	 *
	 * Generalmente maneja la pila de mensajes de Windows (PeekMessage/DispatchMessage)
	 * para mantener la ventana responsiva.
	 */
	void update();

	/**
	 * @brief Renderiza el contenido (placeholder).
	 *
	 * @note La clase Window usualmente no renderiza por sí misma, sino que provee el HWND
	 * para que la SwapChain de DirectX presente la imagen aquí.
	 */
	void render();

	/**
	 * @brief Destruye la ventana y libera el manejador (HWND).
	 */
	void destroy();

public:
	/**
	 * @brief Handle de la ventana Win32.
	 * Identificador fundamental requerido por DirectX para crear la SwapChain.
	 */
	HWND m_hWnd = nullptr;

	/**
	 * @brief Ancho actual del área cliente de la ventana.
	 */
	unsigned int m_width;

	/**
	 * @brief Alto actual del área cliente de la ventana.
	 */
	unsigned int m_height;

private:
	/**
	 * @brief Handle de la instancia de la aplicación (proporcionado por el SO al inicio).
	 */
	HINSTANCE m_hInst = nullptr;

	/**
	 * @brief Estructura que define las coordenadas y dimensiones del rectángulo de la ventana.
	 */
	RECT m_rect;

	/**
	 * @brief Título que aparecerá en la barra superior de la ventana.
	 */
	std::string m_windowName = "Porygon Engine";
};