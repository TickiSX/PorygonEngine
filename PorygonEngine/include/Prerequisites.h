#pragma once

// ----------------------------------------------------------------------------
// Librerías Estándar (STD)
// ----------------------------------------------------------------------------
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <memory>
#include <unordered_map>
#include <type_traits>

// ----------------------------------------------------------------------------
// Librerías de Sistema y Matemáticas
// ----------------------------------------------------------------------------
#include <windows.h>
#include <xnamath.h>

// ----------------------------------------------------------------------------
// Librerías DirectX
// ----------------------------------------------------------------------------
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>

// ----------------------------------------------------------------------------
// Recursos del Proyecto
// ----------------------------------------------------------------------------
#include "Resource.h"
#include "resource.h"

// ----------------------------------------------------------------------------
// Utilidades del Motor (Third Party / Engine)
// ----------------------------------------------------------------------------
#include "EngineUtilities/Vectors/Vector2.h"
#include "EngineUtilities/Vectors/Vector3.h"
#include "EngineUtilities/Memory/TSharedPointer.h"
#include "EngineUtilities/Memory/TWeakPointer.h"
#include "EngineUtilities/Memory/TStaticPtr.h"
#include "EngineUtilities/Memory/TUniquePtr.h"

// ----------------------------------------------------------------------------
// MACROS DE UTILIDAD
// ----------------------------------------------------------------------------

/**
 * @def SAFE_RELEASE
 * @brief Libera un objeto COM de DirectX de manera segura y anula el puntero.
 */
#define SAFE_RELEASE(x) if(x != nullptr) x->Release(); x = nullptr;

 /**
  * @def MESSAGE
  * @brief Envía un mensaje de depuración a la consola de salida de Visual Studio.
  * Útil para rastrear la creación de recursos.
  */
#define MESSAGE(classObj, method, state) \
{ \
    std::wostringstream os_; \
    os_ << classObj << "::" << method << " : " << "[CREATION OF RESOURCE " << ": " << state << "] \n"; \
    OutputDebugStringW(os_.str().c_str()); \
}

  /**
   * @def ERROR
   * @brief Envía un mensaje de error formateado a la consola de depuración.
   * Incluye manejo de excepciones básico para evitar crashes durante el log.
   */
#define ERROR(classObj, method, errorMSG) \
{ \
    try { \
        std::wostringstream os_; \
        os_ << L"ERROR : " << classObj << L"::" << method \
            << L" : " << errorMSG << L"\n"; \
        OutputDebugStringW(os_.str().c_str()); \
    } catch (...) { \
        OutputDebugStringW(L"Failed to log error message.\n"); \
    } \
}

   // ----------------------------------------------------------------------------
   // Estructuras de Datos (Buffers & Vertex)
   // ----------------------------------------------------------------------------

   /**
    * @struct SimpleVertex
    * @brief Estructura básica de vértice usada en el Input Layout.
    */
struct SimpleVertex
{
    XMFLOAT3 Pos; ///< Posición en el espacio 3D (x, y, z).
    XMFLOAT2 Tex; ///< Coordenadas de textura (u, v).
};

/**
 * @struct CBNeverChanges
 * @brief Constant Buffer para datos que raramente cambian (ej. Matriz de Vista/Cámara estática).
 */
struct CBNeverChanges
{
    XMMATRIX mView;
};

/**
 * @struct CBChangeOnResize
 * @brief Constant Buffer para datos que cambian al redimensionar la ventana (ej. Proyección).
 */
struct CBChangeOnResize
{
    XMMATRIX mProjection;
};

/**
 * @struct CBChangesEveryFrame
 * @brief Constant Buffer para datos que se actualizan en cada frame (ej. Mundo, Color).
 */
struct CBChangesEveryFrame
{
    XMMATRIX mWorld;      ///< Matriz de mundo del objeto.
    XMFLOAT4 vMeshColor;  ///< Color base de la malla.
};

// ----------------------------------------------------------------------------
// Enumeraciones
// ----------------------------------------------------------------------------

/**
 * @enum ExtensionType
 * @brief Formatos de imagen soportados para texturas.
 */
enum ExtensionType {
    DDS = 0,
    PNG = 1,
    JPG = 2
};

/**
 * @enum ShaderType
 * @brief Tipos de shaders compilables por el motor.
 */
enum ShaderType {
    VERTEX_SHADER = 0,
    PIXEL_SHADER = 1
};

/**
 * @enum ComponentType
 * @brief Tipos de componentes disponibles en el sistema ECS del juego.
 */
enum ComponentType {
    NONE = 0,       ///< Tipo de componente no especificado o nulo.
    TRANSFORM = 1,  ///< Componente de transformación (posición, rotación, escala).
    MESH = 2,       ///< Componente de malla (geometría).
    MATERIAL = 3    ///< Componente de material (texturas, shaders).
};