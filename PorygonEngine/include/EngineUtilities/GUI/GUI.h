#pragma once

#include "Prerequisites.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include <imgui_internal.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ImGuizmo.h"
#include <string>
#include <vector>

class Viewport;
class Window;
class Device;
class DeviceContext;
class Actor;
class Camera;

// Declaración adelantada para evitar errores en drawViewportPanel
struct ID3D11ShaderResourceView;

/**
 * @class GUI
 * @brief Clase responsable de gestionar la interfaz de usuario del editor.
 */
class GUI {
public:
    GUI() = default;
    ~GUI() = default;

    void awake();
    void init(Window& window, Device& device, DeviceContext& deviceContext);
    void update(Viewport& viewport, Window& window);
    void render();
    void destroy();

    // Paneles Principales
    void drawStudioTopRibbon();
    void drawEditorDockspace();
    void outliner(const std::vector<EU::TSharedPointer<Actor>>& actors);
    void inspectorGeneral(EU::TSharedPointer<Actor> actor);
    void drawViewportPanel(ID3D11ShaderResourceView* viewportSRV);

    // Utilidades de UI
    void ToolBar();
    void closeApp();
    void toolTipData();

    /**
     * @brief Aplica un estilo visual claro con acentos Cyan y Naranja.
     */
    void setupLightTheme();

    /**
     * @brief Aplica un estilo visual transparente tipo Apple/macOS
     */
    void appleLiquidStyle(float opacity, ImVec4 accent);

    // Se añadió el 5to parámetro (displayAsDegrees) para que coincida con GUI.cpp
    void vec3Control(const std::string& label, float* values, float resetValues = 0.0f, float columnWidth = 100.0f, bool displayAsDegrees = false);
    void inspectorContainer(EU::TSharedPointer<Actor> actor);
    void editTransform(Camera& cam, Window& window, EU::TSharedPointer<Actor> actor);
    void drawGizmoToolbar();

    inline void ToFloatArray(const XMMATRIX& mat, float* dest) {
        XMStoreFloat4x4((XMFLOAT4X4*)dest, mat);
    }

    // Método necesario para solicitar guardar la escena desde el BaseApp
    inline bool consumeSaveSceneRequest() {
        bool req = m_requestSaveScene;
        m_requestSaveScene = false;
        return req;
    }

private:
    ImDrawList* m_viewportDrawList = nullptr;

    // Variables para la UI
    char m_hierarchySearchBuffer[128] = "";
    char m_inspectorNameBuffer[128] = "";
    int m_selectedTagIndex = 0;
    int m_selectedLayerIndex = 0;
    float m_debugLightDir[3] = { -0.2f, -1.0f, 1.0f };
    float m_debugLightColor[3] = { 1.0f, 1.0f, 1.0f };

public:
    // ----------------------------------------------------
    // ESTADO PÚBLICO (Variables requeridas por GUI.cpp)
    // ----------------------------------------------------

    // Variables de estado del editor (renombradas para coincidir con el cpp)
    bool show_exit_popup = false;
    bool m_requestSaveScene = false;
    int selectedActorIndex = 0;

    // Variables del Viewport y Gizmos
    bool m_isUsingGizmo = false;
    ImVec2 m_viewportPos = ImVec2(0, 0);
    ImVec2 m_viewportSize = ImVec2(0, 0);
    bool m_viewportHovered = false;
    bool m_viewportFocused = false;
    bool m_viewportActive = false; // Variable que faltaba declarar
};