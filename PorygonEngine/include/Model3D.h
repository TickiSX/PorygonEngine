#pragma once

#include "Prerequisites.h"
#include "IResource.h"
#include "MeshComponent.h"
#include "fbxsdk.h"

/**
 * @enum ModelType
 * @brief Enumera los formatos de archivos de modelos 3D soportados.
 */
enum ModelType {
    OBJ,    ///< Formato Wavefront .obj (Geometría estática básica).
    FBX     ///< Formato Autodesk .fbx (Soporta animaciones, esqueletos, materiales, etc.).
};

/**
 * @class Model3D
 * @brief Recurso que representa un modelo tridimensional cargado desde disco.
 *
 * Esta clase hereda de @c IResource y gestiona la carga y almacenamiento de datos geométricos.
 * Integra el SDK de FBX para procesar archivos complejos y convertirlos en una lista
 * de @c MeshComponent utilizables por el motor.
 */
class Model3D : public IResource {
public:
    /**
     * @brief Constructor de Model3D.
     *
     * Inicializa el recurso, establece el tipo y comienza la carga inmediata del archivo.
     *
     * @param name Nombre/Ruta del archivo del modelo.
     * @param modelType Tipo de formato del modelo (OBJ o FBX).
     */
    Model3D(const std::string& name, ModelType modelType)
        : IResource(name), m_modelType(modelType), lSdkManager(nullptr), lScene(nullptr) {
        SetType(ResourceType::Model3D);
        load(name);
    }

    /**
     * @brief Destructor por defecto.
     */
    ~Model3D() = default;

    // ------------------------------------------------------------------------
    // Métodos de IResource
    // ------------------------------------------------------------------------

    /**
     * @brief Carga el modelo desde la ruta especificada.
     *
     * Dependiendo del @c m_modelType, delegará la carga al procesador correspondiente (ej. FBX SDK).
     *
     * @param path Ruta del archivo en disco.
     * @return true si la carga fue exitosa.
     */
    bool load(const std::string& path) override;

    /**
     * @brief Inicializa los recursos en GPU (si fuera necesario).
     * @return true si la inicialización es correcta.
     */
    bool init() override;

    /**
     * @brief Libera la memoria utilizada por el modelo y el SDK de FBX.
     */
    void unload() override;

    /**
     * @brief Obtiene el tamaño estimado del modelo en bytes.
     * @return Tamaño en bytes.
     */
    size_t getSizeInBytes() const override;

    // ------------------------------------------------------------------------
    // Getters
    // ------------------------------------------------------------------------

    /**
     * @brief Obtiene la lista de mallas que componen este modelo.
     * @return Referencia constante al vector de componentes de malla.
     */
    const std::vector<MeshComponent>& GetMeshes() const { return m_meshes; }

    /**
     * @brief Obtiene los nombres de los archivos de textura asociados al modelo.
     * @return Vector de strings con los nombres de las texturas encontradas.
     */
    std::vector<std::string> GetTextureFileNames() const { return textureFileNames; }

    // ------------------------------------------------------------------------
    // FBX SDK Loader Methods
    // ------------------------------------------------------------------------

    /**
     * @brief Inicializa el gestor de memoria del SDK de FBX.
     * @return true si el manager se creó correctamente.
     */
    bool InitializeFBXManager();

    /**
     * @brief Carga y procesa un archivo FBX completo.
     *
     * Coordina la importación de la escena, el recorrido de nodos y la extracción de geometría.
     *
     * @param filePath Ruta del archivo .fbx.
     * @return Vector de mallas extraídas del archivo.
     */
    std::vector<MeshComponent> LoadFBXModel(const std::string& filePath);

    /**
     * @brief Procesa recursivamente un nodo de la escena FBX.
     *
     * Si el nodo contiene atributos de malla, llama a @c ProcessFBXMesh.
     * Luego recorre los hijos del nodo.
     *
     * @param node Puntero al nodo FBX actual.
     */
    void ProcessFBXNode(FbxNode* node);

    /**
     * @brief Extrae la información geométrica (vértices, índices, UVs) de un nodo.
     *
     * Convierte los datos del formato FBX al formato @c MeshComponent del motor.
     *
     * @param node Puntero al nodo que contiene la malla.
     */
    void ProcessFBXMesh(FbxNode* node);

    /**
     * @brief Procesa los materiales asociados a la superficie.
     *
     * Extrae información de texturas (Diffuse, etc.) del material FBX.
     *
     * @param material Puntero al material FBX.
     */
    void ProcessFBXMaterials(FbxSurfaceMaterial* material);

public:
    // Nota: Variables públicas mantenidas según el código original.
    ModelType m_modelType;                ///< Tipo de formato del modelo.
    std::vector<MeshComponent> m_meshes;  ///< Lista de mallas generadas tras la carga.

private:
    FbxManager* lSdkManager;              ///< Puntero al gestor principal del SDK de FBX.
    FbxScene* lScene;                     ///< Puntero a la escena FBX cargada.
    std::vector<std::string> textureFileNames; ///< Caché de nombres de texturas encontradas.
};