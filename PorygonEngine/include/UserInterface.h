#pragma once

#include "Prerequisites.h"

// --- Bibliotecas de ImGui ---
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <imgui_internal.h>

/**
 * @class UserInterface
 * @brief Envoltorio para la biblioteca Dear ImGui.
 *
 * Esta clase gestiona la inicialización, el ciclo de vida (frames) y el renderizado
 * de la interfaz gráfica de usuario (GUI) para depuración y herramientas del motor.
 * Facilita la creación de ventanas, botones y controles deslizantes.
 */
class UserInterface {
public:
    /**
     * @brief Constructor por defecto.
     * Crea el contexto de ImGui.
     */
    UserInterface();

    /**
     * @brief Destructor.
     * Asegura la limpieza del contexto si no se ha llamado a destroy explícitamente.
     */
    ~UserInterface();

    /**
     * @brief Inicializa el backend de ImGui para Win32 y DirectX 11.
     *
     * @param window        Puntero crudo (void*) al handle de la ventana (HWND).
     * @param device        Puntero al dispositivo DirectX 11.
     * @param deviceContext Puntero al contexto del dispositivo DirectX 11.
     */
    void init(void* window, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

    /**
     * @brief Inicia un nuevo frame de ImGui.
     *
     * Debe llamarse al inicio del bucle de renderizado (antes de definir cualquier ventana UI),
     * pero después de procesar los inputs del sistema.
     */
    void update();

    /**
     * @brief Renderiza los datos de dibujo de ImGui.
     *
     * Debe llamarse al final del pipeline de renderizado, justo antes de presentar el SwapChain.
     */
    void render();

    /**
     * @brief Cierra y libera los recursos de ImGui.
     */
    void destroy();

    /**
     * @brief Dibuja un control personalizado para vectores de 3 componentes (X, Y, Z).
     *
     * Útil para editar posiciones, rotaciones o escalas en un panel de propiedades.
     * Incluye botones para resetear valores individualmente.
     *
     * @param label       Etiqueta identificativa del control.
     * @param values      Puntero a un array de 3 floats (o struct float3) que se modificará.
     * @param resetValues Valor al que se restablecerá el componente si se hace clic en su botón (default 0.0f).
     * @param columnWidth Ancho de la columna de etiquetas para alineación (default 100.0f).
     */
    void vec3Control(std::string label,
        float* values,
        float resetValues = 0.0f,
        float columnWidth = 100.0f);

private:
    // Aquí podrías almacenar estilos personalizados o banderas de configuración interna
};