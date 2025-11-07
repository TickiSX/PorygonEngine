🧩 Porygon Engine ⚙️

Porygon Engine es un motor de renderizado y experimentación 3D escrito en C++, construido sobre DirectX 11.
Creado con fines educativos y de exploración de bajo nivel, implementa un pipeline gráfico en tiempo real, abstrayendo las partes fundamentales del renderizado moderno: dispositivos, contextos, shaders, texturas y buffers.

(algunas códigos hecho con ayuda de sebas y yo ayude a cesar)

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

Descripción de Clases
| **Clase**            | **Descripción**                                                                                                                                               |
| :------------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **BaseApp**          | Núcleo del motor que coordina la inicialización, actualización y renderizado. Controla el ciclo principal de la aplicación y gestiona los módulos esenciales. |
| **Window**           | Encapsula la creación y administración de la ventana Win32, gestionando eventos del sistema y el contexto de presentación.                                    |
| **Device**           | Interfaz principal con la GPU. Se encarga de crear y manejar los recursos gráficos como texturas, shaders y buffers.                                          |
| **DeviceContext**    | Ejecuta los comandos enviados a la GPU, asigna los recursos activos al pipeline y controla las operaciones de dibujo.                                         |
| **SwapChain**        | Administra los buffers de presentación, alternando entre el back buffer y el front buffer para mostrar los fotogramas.                                        |
| **Texture**          | Representa imágenes 2D utilizadas como texturas en los shaders, junto con sus vistas asociadas para lectura o escritura.                                      |
| **RenderTargetView** | Superficie donde se renderiza la imagen final antes de ser mostrada en pantalla. Usualmente se asocia al back buffer.                                         |
| **DepthStencilView** | Búfer que almacena la información de profundidad y stencil, garantizando un renderizado 3D correcto y ordenado.                                               |
| **Buffer**           | Contenedor genérico de datos en GPU. Puede representar vértices, índices o constantes utilizados durante el renderizado.                                      |
| **ShaderProgram**    | Módulo encargado de compilar, vincular y administrar los Vertex y Pixel Shaders escritos en HLSL.                                                             |
| **InputLayout**      | Define la estructura del vértice y cómo los datos del buffer serán interpretados por el Vertex Shader.                                                        |
| **ModelLoader**      | Herramienta encargada de leer y procesar archivos de modelo para generar buffers de geometría listos para renderizar.                                         |
| **MeshComponent**    | Componente visual que agrupa buffers, materiales y texturas para representar una malla dentro del motor.                                                      |

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
Consulta el archivo LICENSE para más detalles.
