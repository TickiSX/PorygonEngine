🧩 Porygon Engine ⚙️

Porygon Engine es un motor de renderizado y experimentación 3D escrito en C++, construido sobre DirectX 11.
Creado con fines educativos y de exploración de bajo nivel, implementa un pipeline gráfico en tiempo real, abstrayendo las partes fundamentales del renderizado moderno: dispositivos, contextos, shaders, texturas y buffers.

🚀 Características Principales

Abstracción de DirectX 11:
Clases limpias y modulares que encapsulan Device, Context, SwapChain, Buffers y vistas de renderizado.

Sistema BaseApp:
Administra el ciclo de vida completo de la aplicación: inicialización, actualización, renderizado y destrucción.

Renderizado Modular:

Creación de ventana nativa (Window)

Gestión de RenderTargetView y DepthStencilView

Búferes para geometría, constantes e índices

Texturizado y shaders HLSL integrados

Componentes y Entidades:

Soporte para entidades con componentes (CShape, Actor, Marker)

Módulo de waypoints y marcadores visuales

Configuración Flexible:

Cambios de estado del rasterizador (Wireframe / Sólido)

Sistema de rotación y animación por frame

Soporte de MSAA y Viewport dinámico

Pipeline simplificado:
Diseño orientado a la enseñanza y la comprensión del flujo GPU-CPU en DirectX 11.

🏗️ Arquitectura del Motor

El motor se organiza bajo una arquitectura orientada a objetos, donde cada clase abstrae un componente esencial del pipeline gráfico.

Clase	Descripción
BaseApp	Núcleo principal del motor. Gestiona el ciclo de vida, bucle de renderizado y actualización de la escena.
Window	Encapsula la creación de ventana y manejo de eventos Win32.
Device	Fabrica recursos de GPU (buffers, shaders, texturas).
DeviceContext	Envía comandos de dibujo y estados al pipeline.
SwapChain	Controla el intercambio de buffers (presentación de imagen).
RenderTargetView	Superficie de renderizado principal (color).
DepthStencilView	Superficie de profundidad y stencil.
Buffer	Representa vertex, index o constant buffers.
ShaderProgram	Administra Vertex/Pixel Shaders y su compilación HLSL.
CShape	Componente renderizable de forma 2D/3D en el sistema ECS.
Actor	Entidad base con comportamiento y representación.
📊 Diagramas de Arquitectura
1️⃣ Diagrama de Flujo de Ejecución
graph TD
    A[WinMain] --> B(BaseApp::init)
    B --> C(Crear Window)
    C --> D(Crear Device y Context)
    D --> E(Crear SwapChain)
    E --> F(Crear RenderTargetView/DepthStencilView)
    F --> G(Cargar Shaders y Buffers)
    G --> H(BaseApp::run)
    H --> I{Bucle Principal}
    I --> J(BaseApp::update)
    J --> K(BaseApp::render)
    K --> L(DeviceContext->DrawIndexed)
    L --> M(SwapChain->Present)
    M --> I
    I -->|WM_QUIT| N(BaseApp::destroy)

2️⃣ Diagrama de Clases
classDiagram
    direction LR
    class BaseApp {
        +init()
        +run()
        +update()
        +render()
        +destroy()
    }
    class Window { +HWND hwnd }
    class Device { +ID3D11Device* device }
    class DeviceContext { +ID3D11DeviceContext* context }
    class SwapChain { +IDXGISwapChain* swap }
    class RenderTargetView { +ID3D11RenderTargetView* rtv }
    class DepthStencilView { +ID3D11DepthStencilView* dsv }
    class ShaderProgram { +Compile() +Bind() }
    class Buffer { +Create() +Bind() }
    class CShape { +Draw() }
    class Actor { +Update() }

    BaseApp o-- Window
    BaseApp o-- Device
    BaseApp o-- DeviceContext
    BaseApp o-- SwapChain
    BaseApp o-- RenderTargetView
    BaseApp o-- DepthStencilView
    BaseApp ..> ShaderProgram
    BaseApp ..> Buffer
    BaseApp ..> CShape
    BaseApp ..> Actor

🎨 Pipeline de Renderizado

Cada cuadro (frame) sigue el flujo definido en BaseApp::render():

Actualización de escena:
Calcula transformaciones, animaciones y lógica del frame.

Limpieza de buffers:
Limpia color y profundidad antes del nuevo renderizado.

Asignación de recursos:

Vertex / Index Buffers

Shaders y Constant Buffers

Texturas activas y samplers

Llamada de dibujo:
DeviceContext->DrawIndexed() para renderizar geometría.

Presentación del frame:
SwapChain->Present() muestra la imagen renderizada en pantalla.

⚙️ Compilar y Ejecutar
📦 Prerrequisitos

Windows 10 / 11

Visual Studio 2019 o superior

DirectX SDK (Junio 2010) o integración con el Windows SDK actual

Compatibilidad con GPU DirectX 11+

🧱 Pasos de Compilación

Clonar el repositorio:

git clone https://github.com/TickiSX/PorygonEngine.git
cd PorygonEngine


Abrir la solución PorygonEngine.sln en Visual Studio.

Configurar dependencias de DirectX (Include y Lib).

Compilar en modo Debug o Release.

Ejecutar el proyecto principal (Tutorial07.cpp o BaseApp demo).

🧠 Licencia

Este proyecto se distribuye bajo la Licencia MIT.
Consulta el archivo LICENSE
 para más detalles.
