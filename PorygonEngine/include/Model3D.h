#pragma once
#include "Prerequisites.h"
#include "IResource.h"
#include "MeshComponent.h"
#include "fbxsdk.h"

/**
 * @enum ModelType
 * @brief Define los formatos de archivo de modelo 3D soportados.
 */
enum
    ModelType {
    OBJ, ///< Formato Wavefront OBJ.
    FBX  ///< Formato Autodesk FBX.
};

/**
 * @class Model3D
 * @brief Recurso que representa un modelo tridimensional compuesto por una o varias mallas.
 *
 * Esta clase hereda de @c IResource y se encarga de la carga y gestión de datos
 * geométricos desde archivos externos (FBX/OBJ) o definiciones manuales (Skybox).
 * Utiliza el SDK de FBX para procesar nodos, mallas y materiales.
 */
class
    Model3D : public IResource {
public:
    /**
     * @brief Constructor para cargar un modelo desde un archivo.
     * @param name      Nombre identificador del recurso.
     * @param modelType Tipo de formato del archivo (@c ModelType).
     */
    Model3D(const std::string& name, ModelType modelType)
        : IResource(name),
        m_modelType(modelType),
        lSdkManager(nullptr),
        lScene(nullptr) {
        SetType(ResourceType::Model3D);
        load(name);
    }

    /**
     * @brief Constructor para crear un modelo manual (específico para Skybox).
     * @param name     Nombre identificador del recurso.
     * @param vertices Arreglo de vértices de tipo @c SkyboxVertex.
     * @param indices  Arreglo de índices.
     */
    Model3D(const std::string& name,
        const SkyboxVertex vertices[],
        const unsigned int indices[])
        : IResource(name) {
        MeshComponent mesh;
        mesh.m_skyVertex.assign(vertices, vertices + 8);
        mesh.m_index.assign(indices, indices + 36);
        mesh.m_numIndex = mesh.m_index.size();
        SetType(ResourceType::Model3D);
        m_meshes.push_back(mesh);
    }

    /**
     * @brief Destructor virtual por defecto.
     */
    ~Model3D() = default;

    /**
     * @brief Carga el modelo desde la ruta especificada.
     * @param path Ruta del archivo en disco.
     * @return @c true si la carga fue exitosa.
     */
    bool
        load(const std::string& path) override;

    /**
     * @brief Inicializa los recursos del modelo para su uso en la GPU.
     * @return @c true si la inicialización tuvo éxito.
     */
    bool
        init() override;

    /**
     * @brief Libera los datos del modelo cargados en memoria.
     */
    void
        unload() override;

    /**
     * @brief Obtiene el tamaño aproximado del modelo en bytes.
     * @return Tamaño en bytes.
     */
    size_t
        getSizeInBytes() const override;

    /**
     * @brief Obtiene la lista de mallas que componen el modelo.
     * @return Referencia constante al vector de @c MeshComponent.
     */
    const std::vector<MeshComponent>&
        GetMeshes() const { return m_meshes; }

    /* FBX MODEL LOADER */

    /**
     * @brief Inicializa el SDK Manager de FBX.
     * @return @c true si el manager se creó correctamente.
     */
    bool
        InitializeFBXManager();

    /**
     * @brief Carga y parsea un archivo FBX.
     * @param filePath Ruta del archivo FBX.
     * @return Vector de componentes de malla extraídos.
     */
    std::vector<MeshComponent>
        LoadFBXModel(const std::string& filePath);

    /**
     * @brief Procesa un nodo de la jerarquía FBX de forma recursiva.
     * @param node Puntero al nodo FBX actual.
     */
    void
        ProcessFBXNode(FbxNode* node);

    /**
     * @brief Extrae la geometría de un nodo FBX de tipo Mesh.
     * @param node Puntero al nodo que contiene la malla.
     */
    void
        ProcessFBXMesh(FbxNode* node);

    /**
     * @brief Procesa y extrae rutas de texturas de los materiales FBX.
     * @param material Puntero al material de superficie FBX.
     */
    void
        ProcessFBXMaterials(FbxSurfaceMaterial* material);

    /**
     * @brief Obtiene las rutas de archivos de textura detectadas en el modelo.
     * @return Vector de strings con las rutas de textura.
     */
    std::vector<std::string>
        GetTextureFileNames() const { return textureFileNames; }

private:
    FbxManager* lSdkManager; ///< Puntero al gestor del SDK de FBX.
    FbxScene* lScene;      ///< Puntero a la escena FBX cargada.

    /**
     * @brief Lista de nombres de archivos de textura asociados al modelo.
     */
    std::vector<std::string> textureFileNames;

public:
    ModelType m_modelType;             ///< Formato del modelo cargado.
    std::vector<MeshComponent> m_meshes; ///< Lista de mallas del modelo.
};