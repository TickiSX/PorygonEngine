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
#include "UserInterface.h"
#include "ECS/Actor.h"

/**
 * @class BaseApp
 * @brief Clase principal que gestiona el ciclo de vida de la aplicación.
 *
 * Esta clase encapsula la inicialización de la ventana y DirectX, así como
 * el bucle principal (Main Loop) que coordina la actualización (Update) y
 * el renderizado (Render) de la escena.
 */
class BaseApp {
public:
    /**
     * @brief Constructor de la aplicación.
     *
     * @param hInst     Instancia de la aplicación (handle de Windows).
     * @param nCmdShow  Parámetro que indica cómo se debe mostrar la ventana.
     */
    BaseApp(HINSTANCE hInst, int nCmdShow);

    /**
     * @brief Destructor.
     * Llama internamente a destroy() para asegurar la liberación de recursos.
     */
    ~BaseApp() { destroy(); }

    HRESULT awake();
    /**
     * @brief Inicia el bucle principal de mensajes de Windows.
     *
     * @param hInst     Instancia de la aplicación.
     * @param nCmdShow  Comando de visualización.
     * @return Código de salida de la aplicación (WPARAM del mensaje WM_QUIT).
     */
    int run(HINSTANCE hInst, int nCmdShow);

    /**
     * @brief Inicializa todos los subsistemas del motor.
     *
     * Incluye la creación de la ventana, el dispositivo DirectX, swap chain,
     * vistas, shaders y la carga inicial de recursos/actores.
     *
     * @return S_OK si todo se inicializó correctamente, o un código de error HRESULT.
     */
    HRESULT init();

    /**
     * @brief Actualiza la lógica del juego.
     *
     * Se llama una vez por frame. Aquí se calculan transformaciones, física
     * y lógica de juego.
     *
     * @param deltaTime Tiempo transcurrido (en segundos) desde el último frame.
     */
    void update(float deltaTime);

    /**
     * @brief Renderiza la escena actual.
     *
     * Limpia las vistas, configura el pipeline, dibuja los actores y presenta
     * el back buffer (Swap Chain).
     */
    void render();

    /**
     * @brief Libera todos los recursos y memoria al cerrar la aplicación.
     */
    void destroy();

private:
    /**
     * @brief Procedimiento de ventana (Callback estático).
     * Procesa los eventos del sistema operativo (teclado, mouse, cerrar ventana, etc.).
     */
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    // ------------------------------------------------------------------------
    // Core DirectX & Window
    // ------------------------------------------------------------------------
    Window              m_window;           ///< Abstracción de la ventana de SO.
    Device              m_device;           ///< Dispositivo gráfico (creación de recursos).
    DeviceContext       m_deviceContext;    ///< Contexto inmediato (comandos de dibujo).
    SwapChain           m_swapChain;        ///< Cadena de intercambio (Front/Back buffer).
    Viewport            m_viewport;         ///< Configuración del área de renderizado.

    // ------------------------------------------------------------------------
    // Vistas y Buffers de Renderizado
    // ------------------------------------------------------------------------
    Texture             m_backBuffer;       ///< Textura del buffer trasero.
    RenderTargetView    m_renderTargetView; ///< Vista para dibujar en el Back Buffer.
    Texture             m_depthStencil;     ///< Textura de profundidad.
    DepthStencilView    m_depthStencilView; ///< Vista para el buffer de profundidad/stencil.

    // ------------------------------------------------------------------------
    // Shaders y Estados
    // ------------------------------------------------------------------------
    ShaderProgram       m_shaderProgram;    ///< Gestor de Vertex y Pixel Shaders.
    SamplerState        m_samplerState;     ///< Estado de muestreo para texturas.

    // ------------------------------------------------------------------------
    // Constant Buffers (Comunicación CPU -> GPU)
    // ------------------------------------------------------------------------
    Buffer              m_cbNeverChanges;       ///< Datos estáticos (ej. View Matrix fija).
    Buffer              m_cbChangeOnResize;     ///< Datos que cambian al redimensionar (ej. Proyección).
    Buffer              m_cbChangesEveryFrame;  ///< Datos por frame (ej. World Matrix, Tiempo).

    // Estructuras de datos locales para los buffers
    CBNeverChanges      cbNeverChanges;
    CBChangeOnResize    cbChangesOnResize;
    CBChangesEveryFrame cb;

    // ------------------------------------------------------------------------
    // Recursos Específicos (CyberGun)
    // ------------------------------------------------------------------------
    Texture             m_cyberGunAlbedo;       ///< Textura de color base.
    Texture             m_cyberGunNormal;       ///< Mapa de normales.
    Texture             m_cyberGunMetallic;     ///< Mapa metálico (PBR).
    Texture             m_cyberGunGlossiness;   ///< Mapa de brillo/suavidad (PBR).

    EU::TSharedPointer<Actor> m_cyberGun;       ///< Puntero inteligente al actor principal.

    // ------------------------------------------------------------------------
    // Escena y Lógica Global
    // ------------------------------------------------------------------------
    std::vector<EU::TSharedPointer<Actor>> m_actors; ///< Lista de actores en la escena.
    Model3D* m_model;                ///< Recurso del modelo 3D cargado.
    UserInterface       UI;                     ///< Sistema de interfaz de usuario (ImGui, etc.).

    // Matrices Globales
    XMMATRIX            m_World;        ///< Matriz de Mundo global.
    XMMATRIX            m_View;         ///< Matriz de Vista (Cámara).
    XMMATRIX            m_Projection;   ///< Matriz de Proyección.
    XMFLOAT4            m_vMeshColor;   ///< Color base para mallas (debug/tint).
};