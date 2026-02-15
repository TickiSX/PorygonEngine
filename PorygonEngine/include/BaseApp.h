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

// Declaración externa necesaria para que Windows pueda enviar inputs (teclado/mouse) a ImGui.
extern IMGUI_IMPL_API
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/**
 * @class BaseApp
 * @brief Clase principal (Motor) de la aplicación.
 * * * Esta clase actúa como el orquestador central. Sus responsabilidades son:
 * 1. Inicializar el sistema operativo (Ventana) y la API Gráfica (DirectX 11).
 * 2. Gestionar el bucle principal (Main Loop) de mensajes de Windows.
 * 3. Ejecutar la lógica de actualización (Update) y renderizado (Render) en cada frame.
 * 4. Administrar los recursos globales (Cámara, Escena, GUI).
 */
class BaseApp {
public:
    BaseApp() = default;

    /** * @brief Destructor que asegura la llamada a destroy() para liberar recursos COM.
     */
    ~BaseApp() { destroy(); }

    /**
     * @brief Configuración temprana.
     * * Se ejecuta antes de crear la ventana o el dispositivo gráfico.
     * * Útil para cargar configuraciones o reservar memoria básica.
     * @return HRESULT Estado de la operación (S_OK si todo fue bien).
     */
    HRESULT awake();

    /**
     * @brief Inicia el bucle principal de la aplicación.
     * * Contiene el `while(GetMessage)` estándar de Windows.
     * * Llama a `init()`, y luego cíclicamente a `update()` y `render()`.
     * @param hInst Instancia de la aplicación (WinMain).
     * @param nCmdShow Comando de visualización inicial de la ventana.
     * @return Código de salida de la aplicación.
     */
    int run(HINSTANCE hInst, int nCmdShow);

    /**
     * @brief Inicialización pesada de sistemas gráficos.
     * * Crea el Device, SwapChain, RenderTargets, Shaders, Texturas y carga los modelos iniciales.
     * @return HRESULT Estado de la inicialización.
     */
    HRESULT init();

    /**
     * @brief Actualización lógica por frame.
     * * Actualiza la cámara, el grafo de escena, las animaciones y la GUI.
     * @param deltaTime Tiempo transcurrido en segundos desde el último frame.
     */
    void update(float deltaTime);

    /**
     * @brief Renderizado por frame.
     * * Limpia el BackBuffer, configura el pipeline de dibujo y presenta la imagen final (SwapBuffer).
     */
    void render();

    /**
     * @brief Liberación de recursos.
     * * Libera memoria y objetos COM de DirectX en orden inverso a su creación.
     */
    void destroy();

private:
    /**
     * @brief Callback estático para manejar mensajes de Windows (Input, Resize, Close).
     * * Es vital para que la ventana responda y para pasar inputs a ImGui.
     */
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    // =========================================================
    // Sistemas Core (Infraestructura DirectX 11)
    // =========================================================
    Window                  m_window;           ///< Ventana del sistema operativo.
    Device                  m_device;           ///< Interfaz para crear recursos (texturas, buffers).
    DeviceContext           m_deviceContext;    ///< Interfaz para enviar comandos de dibujo a la GPU.
    SwapChain               m_swapChain;        ///< Cadena de intercambio (Double Buffering).
    Texture                 m_backBuffer;       ///< Textura donde se dibuja el frame actual.
    RenderTargetView        m_renderTargetView; ///< Vista para que el OM (Output Merger) escriba en el BackBuffer.
    Texture                 m_depthStencil;     ///< Textura para el Z-Buffer (profundidad).
    DepthStencilView        m_depthStencilView; ///< Vista del Z-Buffer.
    Viewport                m_viewport;         ///< Define el área de dibujo en pantalla.
    ShaderProgram           m_shaderProgram;    ///< Gestión de Vertex y Pixel Shaders.

    // =========================================================
    // Buffers y Recursos Gráficos
    // =========================================================
    Buffer                  m_cbNeverChanges;   ///< Buffer constante para datos estáticos (ej: luces fijas).
    Buffer                  m_cbChangeOnResize; ///< Buffer constante que cambia al redimensionar (ej: Matriz Proyección).
    Texture                 m_cyberGunAlbedo;   ///< Textura específica (ejemplo).
    Texture                 m_skyboxTex;        ///< Textura de entorno/fondo.
    Model3D* m_model = nullptr;                 ///< Puntero crudo a un modelo 3D (gestionar con cuidado).

    // =========================================================
    // Lógica y Escena
    // =========================================================

    /** @brief Cámara principal que reemplaza las matrices de vista/proyección manuales. */
    Camera                  m_camera;

    /** @brief Grafo de escena para gestionar jerarquías de entidades. */
    SceneGraph              m_sceneGraph;

    /** @brief Lista maestra de actores (Smart Pointers para gestión automática de memoria). */
    std::vector<EU::TSharedPointer<Actor>> m_actors;

    // Actores específicos (Referencias directas para acceso rápido)
    EU::TSharedPointer<Actor> m_cyberGun;
    EU::TSharedPointer<Actor> m_Character;

    // =========================================================
    // Estructuras de datos (Mapping CPU -> GPU)
    // =========================================================
    CBChangeOnResize        cbChangesOnResize;  ///< Estructura C++ que coincide con el cbuffer HLSL.
    CBNeverChanges          cbNeverChanges;     ///< Estructura C++ que coincide con el cbuffer HLSL.

    // =========================================================
    // Interfaz de Usuario
    // =========================================================
    GUI                     m_gui;              ///< Sistema de editor/UI (ImGui).
};