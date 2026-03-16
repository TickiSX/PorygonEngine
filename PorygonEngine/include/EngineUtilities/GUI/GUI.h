#pragma once
#include "Prerequisites.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include <imgui_internal.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ImGuizmo.h"

class Viewport;
class Window;
class Device;
class DeviceContext;
class Actor;
class Camera;

/**
 * @class GUI
 * @brief Clase responsable de gestionar la interfaz de usuario del editor.
 *
 * Encapsula la integración de ImGui, ImGuizmo y la lógica de paneles como el Inspector,
 * Outliner y el Viewport del editor. Gestiona el Dockspace y los estilos visuales.
 */
class
    GUI {
public:
    /**
     * @brief Constructor por defecto.
     */
    GUI() = default;

    /**
     * @brief Destructor por defecto.
     */
    ~GUI() = default;

    /**
     * @brief Preparación inicial de la lógica de GUI.
     */
    void
        awake();

    /**
     * @brief Inicializa los contextos de ImGui para Win32 y DirectX 11.
     *
     * @param window        Ventana principal de la aplicación.
     * @param device        Dispositivo de Direct3D 11.
     * @param deviceContext Contexto de dispositivo de Direct3D 11.
     */
    void
        init(Window& window, Device& device, DeviceContext& deviceContext);

    /**
     * @brief Actualiza la lógica de los paneles y prepara el nuevo frame de ImGui.
     *
     * @param viewport Referencia al viewport para gestionar dimensiones.
     * @param window   Referencia a la ventana para eventos de entrada.
     */
    void
        update(Viewport& viewport, Window& window);

    /**
     * @brief Finaliza el frame de ImGui y envía los datos de renderizado a la GPU.
     */
    void
        render();

    /**
     * @brief Libera los contextos de ImGui y recursos asociados.
     */
    void
        destroy();

    /**
     * @brief Dibuja la barra de herramientas principal.
     */
    void
        ToolBar();

    /**
     * @brief Gestiona el popup de confirmación de salida.
     */
    void
        closeApp();

    /**
     * @brief Muestra datos informativos de herramientas (tooltips).
     */
    void
        toolTipData();

    /**
     * @brief Aplica un estilo visual personalizado inspirado en Apple Liquid.
     * * @param opacity Nivel de transparencia de la interfaz.
     * @param accent  Color de acento para elementos interactivos.
     */
    void
        appleLiquidStyle(float opacity, ImVec4 accent);

    /**
     * @brief Crea controles deslizantes personalizados para vectores de 3 componentes.
     *
     * @param label       Etiqueta del control.
     * @param values      Puntero al array de floats (X, Y, Z).
     * @param resetValues Valor al que se reseteará cada componente.
     * @param columnWidth Ancho de la columna de la etiqueta.
     */
    void
        vec3Control(const std::string& label, float* values, float resetValues = 0.0f, float columnWidth = 100.0f);

    /**
     * @brief Dibuja el panel de propiedades generales del actor seleccionado.
     * @param actor Puntero compartido al actor a inspeccionar.
     */
    void
        inspectorGeneral(EU::TSharedPointer<Actor> actor);

    /**
     * @brief Dibuja el contenedor del inspector (componentes, etc).
     * @param actor Puntero compartido al actor.
     */
    void
        inspectorContainer(EU::TSharedPointer<Actor> actor);

    /**
     * @brief Dibuja la jerarquía de actores de la escena.
     * @param actors Vector de actores registrados en el sistema.
     */
    void
        outliner(const std::vector<EU::TSharedPointer<Actor>>& actors);

    /**
     * @brief Gestiona la lógica del Gizmo (traslación, rotación, escala) en el espacio 3D.
     *
     * @param cam    Cámara activa para calcular la proyección del Gizmo.
     * @param window Ventana para coordenadas de ratón.
     * @param actor  Actor sobre el cual se aplicará la transformación.
     */
    void
        editTransform(Camera& cam, Window& window, EU::TSharedPointer<Actor> actor);

    /**
     * @brief Dibuja la barra de herramientas flotante del Gizmo.
     */
    void
        drawGizmoToolbar();

    /**
     * @brief Dibuja la cinta superior (Ribbon) de la aplicación.
     */
    void
        drawStudioTopRibbon();

    /**
     * @brief Dibuja el panel que contiene la textura del viewport renderizado.
     * @param viewportSRV Vista de recurso de shader que contiene la escena renderizada.
     */
    void
        drawViewportPanel(ID3D11ShaderResourceView* viewportSRV);

    /**
     * @brief Configura el espacio de acoplamiento (Dockspace) principal del editor.
     */
    void
        drawEditorDockspace();

    /**
     * @brief Utilidad para convertir una matriz @c XMMATRIX a un array de floats.
     * @param mat  Matriz de origen.
     * @param dest Puntero al array de destino (mínimo 16 floats).
     */
    inline void
        ToFloatArray(const XMMATRIX& mat, float* dest) {
        XMStoreFloat4x4((XMFLOAT4X4*)dest, mat);
    }

private:
    bool m_checkboxValue = true;
    bool m_checkboxValue2 = false;
    std::vector<const char*> m_objectsNames;
    std::vector<const char*> m_tooltips;
    bool m_showExitPopup = false;
    ImDrawList* m_viewportDrawList = nullptr;
    bool m_viewportActive = false;

public:
    bool m_isUsingGizmo = false;        ///< Indica si el Gizmo de transformación está siendo manipulado.
    int m_selectedActorIndex = -1;      ///< Índice del actor actualmente seleccionado en el Outliner.
    ImVec2 m_viewportPos = ImVec2(0, 0); ///< Posición actual de la ventana del viewport.
    ImVec2 m_viewportSize = ImVec2(0, 0);///< Dimensiones de la ventana del viewport.
    bool m_viewportHovered = false;     ///< Indica si el ratón está sobre el viewport.
    bool m_viewportFocused = false;     ///< Indica si el viewport tiene el foco del teclado.
};