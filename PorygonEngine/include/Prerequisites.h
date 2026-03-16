#pragma once

// Librerias STD
#include <string>
#include <sstream>
#include <vector>
#include <windows.h>
#include <xnamath.h>
#include <thread>
#include <memory>
#include <unordered_map>
#include <type_traits>
#include <array>

// Librerias DirectX
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include "Resource.h"
#include "resource.h"

// Third Party Libraries
#include "EngineUtilities/Vectors/Vector2.h"
#include "EngineUtilities/Vectors/Vector3.h"
#include "EngineUtilities\Memory\TSharedPointer.h"
#include "EngineUtilities\Memory\TWeakPointer.h"
#include "EngineUtilities\Memory\TStaticPtr.h"
#include "EngineUtilities\Memory\TUniquePtr.h"

//--------------------------------------------------------------------------------------
// MACROS
//--------------------------------------------------------------------------------------

/**
 * @brief Libera un recurso de DirectX y lo establece a @c nullptr.
 */
#define SAFE_RELEASE(x) if(x != nullptr) x->Release(); x = nullptr;

 /**
  * @brief Registra la creación exitosa o fallida de un recurso en la consola de debug.
  */
#define MESSAGE( classObj, method, state )   \
{                                            \
   std::wostringstream os_;                  \
   os_ << classObj << "::" << method << " : " << "[CREATION OF RESOURCE " << ": " << state << "] \n"; \
   OutputDebugStringW( os_.str().c_str() );  \
}

  /**
   * @brief Registra un error detallado en la consola de debug.
   */
#define ERROR(classObj, method, errorMSG)                     \
{                                                             \
    try {                                                     \
        std::wostringstream os_;                              \
        os_ << L"ERROR : " << classObj << L"::" << method     \
            << L" : " << errorMSG << L"\n";                   \
        OutputDebugStringW(os_.str().c_str());                \
    } catch (...) {                                           \
        OutputDebugStringW(L"Failed to log error message.\n");\
    }                                                         \
}

   //--------------------------------------------------------------------------------------
   // Structures
   //--------------------------------------------------------------------------------------

   /**
    * @struct SimpleVertex
    * @brief Representa un vértice estándar con datos de iluminación y texturizado.
    */
struct
    SimpleVertex {
    EU::Vector3 Position;
    EU::Vector3 Normal;
    EU::Vector3 Tangent;
    EU::Vector3 Bitangent;
    EU::Vector2 TextureCoordinate;
};

/**
 * @struct SkyboxVertex
 * @brief Estructura simplificada para vértices de un Skybox.
 */
struct
    SkyboxVertex {
    float x, y, z;
};

/**
 * @struct CBNeverChanges
 * @brief Constant Buffer para datos que no varían durante la ejecución.
 */
struct
    CBNeverChanges {
    XMMATRIX mView;
};

/**
 * @struct CBSkybox
 * @brief Constant Buffer específico para el renderizado de Skybox.
 */
struct
    CBSkybox {
    XMMATRIX mviewProj;
};

/**
 * @struct CBChangeOnResize
 * @brief Constant Buffer que se actualiza al redimensionar la ventana.
 */
struct
    CBChangeOnResize {
    XMMATRIX mProjection;
};

/**
 * @struct CBMain
 * @brief Constant Buffer principal para el vertex y pixel shader.
 * @note Alineado a 16 bytes según requerimientos de Direct3D.
 */
struct
    CBMain {
    XMFLOAT4X4 View;
    XMFLOAT4X4 Projection;
    EU::Vector3 CameraPos;
    float pad0;
    EU::Vector3 LightDir;
    float pad1;
    EU::Vector3 LightColor;
    float pad2;
};

/**
 * @struct CBChangesEveryFrame
 * @brief Constant Buffer para datos que cambian en cada frame (como matrices de mundo).
 */
struct
    CBChangesEveryFrame {
    XMMATRIX mWorld;
    XMFLOAT4 vMeshColor;
};

//--------------------------------------------------------------------------------------
// Enums
//--------------------------------------------------------------------------------------

/**
 * @enum ExtensionType
 * @brief Formatos de imagen soportados por el motor.
 */
enum
    ExtensionType {
    DDS = 0,
    PNG = 1,
    JPG = 2
};

/**
 * @enum ShaderType
 * @brief Identificadores para etapas del pipeline de shaders.
 */
enum
    ShaderType {
    VERTEX_SHADER = 0,
    PIXEL_SHADER = 1
};

/**
 * @enum ComponentType
 * @brief Tipos de componentes disponibles en el sistema ECS.
 */
enum
    ComponentType {
    NONE = 0,      ///< Tipo de componente no especificado.
    TRANSFORM = 1, ///< Componente de transformación (Pos/Rot/Sca).
    MESH = 2,      ///< Componente de malla geométrica.
    MATERIAL = 3,  ///< Componente de material y sombreado.
    HIERARCHY = 4  ///< Componente de jerarquía padre-hijo.
};