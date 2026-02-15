#pragma once
#include "Prerequisites.h"

#include "imgui.h"
#include <imgui_internal.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ImGuizmo.h"

// Forward Declarations
class Viewport;
class Window;
class Device;
class DeviceContext;
class Actor;

/**
 * @class GUI
 * @brief Sistema de Interfaz de Usuario (Editor) basado en ImGui.
 * * Gestiona todas las ventanas del editor: Inspector, Outliner, Toolbar y Gizmos.
 * * Se encarga de la inicialización del backend de ImGui para Win32 y DirectX 11.
 */
class GUI {
public:
    GUI() = default;
    ~GUI() = default;

    /**
     * @brief Configuración inicial previa al contexto gráfico.
     */
    void awake();

    /**
     * @brief Inicializa ImGui y conecta los backends de plataforma y renderizado.
     * @param window Referencia a la ventana de OS (Win32) para inputs.
     * @param device Dispositivo DirectX para crear recursos de fuentes/texturas.
     * @param deviceContext Contexto para comandos de dibujo.
     */
    void init(Window& window, Device& device, DeviceContext& deviceContext);

    /**
     * @brief Inicia el frame de ImGui y construye la lógica de la UI.
     * * Aquí se define CÓMO se ven las ventanas, pero no se renderizan aún.
     * * Actualizado: Requiere Viewport para calcular inputs relativos a la vista de la escena.
     * @param viewport Datos de la vista (cámara editor).
     * @param window Ventana principal.
     */
    void update(Viewport& viewport, Window& window);

    /**
     * @brief Ejecuta el renderizado final de ImGui (ImGui::Render).
     * * Envía los comandos de dibujo a la GPU a través del DeviceContext.
     */
    void render();

    /**
     * @brief Libera los recursos de ImGui y los backends asociados.
     */
    void destroy();

    /**
     * @brief Dibuja la barra de herramientas superior (File, Edit, etc.) o botones rápidos.
     */
    void ToolBar();

    /**
     * @brief Gestiona la lógica para cerrar la aplicación (ej: popup de confirmación).
     */
    void closeApp();

    /**
     * @brief Define o muestra los tooltips (información flotante) de la UI.
     */
    void toolTipData();

    /**
     * @brief Aplica un tema visual personalizado (Estilo "Liquid").
     * @param opacity Opacidad global de las ventanas (0.0f a 1.0f).
     * @param accent Color de acento principal (títulos, selecciones).
     */
    void appleLiquidStyle(float opacity /*0..1f*/, ImVec4 accent /*=#0A84FF*/);

    /**
     * @brief Widget personalizado para controlar vectores de 3 componentes (X, Y, Z).
     * * Útil para Transform (Posición, Rotación, Escala).
     * @param label Etiqueta del control (ej: "Position").
     * @param values Puntero al array de 3 floats a modificar.
     * @param resetValues Valor al que se resetea si se hace doble click o click derecho.
     * @param columnWidth Ancho de la etiqueta.
     */
    void vec3Control(const std::string& label,
        float* values,
        float resetValues = 0.0f,
        float columnWidth = 100.0f);

    /**
     * @brief Panel Inspector: Muestra propiedades generales del Actor seleccionado.
     * @param actor Puntero compartido al actor a inspeccionar.
     */
    void inspectorGeneral(EU::TSharedPointer<Actor> actor);

    /**
     * @brief Panel Contenedor: Agrupa componentes o secciones del actor.
     */
    void inspectorContainer(EU::TSharedPointer<Actor> actor);

    /**
     * @brief Panel Outliner (Jerarquía): Lista todos los actores de la escena.
     * * Permite seleccionar actores haciendo clic en ellos.
     * @param actors Lista de todos los actores en la escena actual.
     */
    void outliner(const std::vector<EU::TSharedPointer<Actor>>& actors);

    /**
     * @brief Dibuja y gestiona el Gizmo de manipulación 3D (Traslación/Rotación/Escala).
     * * Utiliza la librería ImGuizmo para manipular la matriz de mundo del actor.
     * @param view Matriz de vista de la cámara actual.
     * @param projection Matriz de proyección de la cámara actual.
     * @param actor Actor que se va a transformar.
     */
    void editTransform(const XMMATRIX& view, const XMMATRIX& projection, EU::TSharedPointer<Actor> actor);

    /**
     * @brief Dibuja la barra de herramientas específica para cambiar modos de Gizmo (T, R, S).
     */
    void drawGizmoToolbar();

    /**
     * @brief Helper: Convierte una matriz DirectX (XMMATRIX/SIMD) a un array de floats plano.
     * * Necesario porque ImGui/ImGuizmo no entienden tipos SIMD de DirectX, solo punteros float*.
     * @param mat Matriz de entrada (DirectX Math).
     * @param dest Puntero al array de destino (debe tener espacio para 16 floats).
     */
    void ToFloatArray(const XMMATRIX& mat, float* dest) {
        XMFLOAT4X4 temp;
        XMStoreFloat4x4(&temp, mat);
        memcpy(dest, &temp, sizeof(float) * 16);
    }

private:
    bool checkboxValue = true;
    bool checkboxValue2 = false;

    /** @brief Nombres para mostrar en listas de UI (caché o temporales). */
    std::vector<const char*> m_objectsNames;

    /** @brief Textos de ayuda para los elementos de la UI. */
    std::vector<const char*> m_tooltips;

    /** @brief Controla si el popup de salida está abierto. */
    bool show_exit_popup = false;

public:
    /** @brief Índice del actor actualmente seleccionado en el Outliner (-1 si ninguno). */
    int selectedActorIndex = -1;
};