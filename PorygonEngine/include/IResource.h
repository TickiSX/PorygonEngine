#pragma once
#include "Prerequisites.h"
#include <string>

/**
 * @enum ResourceType
 * @brief Enumera los diferentes tipos de recursos gestionados por el motor.
 */
enum class
    ResourceType {
    Unknown,    ///< Tipo desconocido o no inicializado.
    Model3D,    ///< Modelo tridimensional (malla).
    Texture,    ///< Textura o imagen.
    Sound,      ///< Archivo de audio.
    Shader,     ///< Programa de shader.
    Material    ///< Material de renderizado.
};

/**
 * @enum ResourceState
 * @brief Define los posibles estados del ciclo de vida de un recurso.
 */
enum class
    ResourceState {
    Unloaded,   ///< El recurso no está cargado en memoria.
    Loading,    ///< El recurso está en proceso de carga.
    Loaded,     ///< El recurso se ha cargado correctamente y está listo.
    Failed      ///< Falló la carga o inicialización del recurso.
};

/**
 * @class IResource
 * @brief Interfaz base para todos los recursos del sistema.
 *
 * Define la estructura básica y el contrato que deben cumplir todos los recursos
 * (texturas, modelos, sonidos, etc.), incluyendo gestión de estados e identificación.
 */
class
    IResource {
public:
    /**
     * @brief Constructor.
     * @param name Nombre identificador del recurso.
     */
    IResource(const std::string& name)
        : m_name(name),
        m_filePath(""),
        m_type(ResourceType::Unknown),
        m_state(ResourceState::Unloaded),
        m_id(GenerateID()) {
    }

    /**
     * @brief Destructor virtual.
     */
    virtual
        ~IResource() = default;

    /**
     * @brief Inicializa el recurso en la API gráfica (GPU).
     * @return @c true si la inicialización fue exitosa, @c false en caso contrario.
     */
    virtual bool
        init() = 0;

    /**
     * @brief Carga el recurso desde el almacenamiento en disco.
     * @param filename Ruta del archivo a cargar.
     * @return @c true si la carga fue exitosa, @c false si falló.
     */
    virtual bool
        load(const std::string& filename) = 0;

    /**
     * @brief Libera la memoria ocupada por el recurso.
     */
    virtual void
        unload() = 0;

    /**
     * @brief Obtiene el tamaño del recurso en memoria.
     * @return Tamaño en bytes (útil para el profiler de memoria).
     */
    virtual size_t
        getSizeInBytes() const = 0;

    // --- Setters ---

    /**
     * @brief Establece la ruta del archivo.
     * @param path Ruta física del archivo.
     */
    void
        SetPath(const std::string& path) { m_filePath = path; }

    /**
     * @brief Establece el tipo de recurso.
     * @param t Tipo de recurso (@c ResourceType).
     */
    void
        SetType(ResourceType t) { m_type = t; }

    /**
     * @brief Cambia el estado actual del recurso.
     * @param s Nuevo estado (@c ResourceState).
     */
    void
        SetState(ResourceState s) { m_state = s; }

    // --- Getters ---

    /**
     * @brief Obtiene el nombre del recurso.
     * @return Nombre identificador.
     */
    const std::string&
        GetName() const { return m_name; }

    /**
     * @brief Obtiene la ruta del archivo.
     * @return String con la ruta.
     */
    const std::string&
        GetPath() const { return m_filePath; }

    /**
     * @brief Obtiene el tipo de recurso.
     * @return Tipo de recurso.
     */
    ResourceType
        GetType() const { return m_type; }

    /**
     * @brief Obtiene el estado actual del recurso.
     * @return Estado actual (@c Loaded, @c Unloaded, etc.).
     */
    ResourceState
        GetState() const { return m_state; }

    /**
     * @brief Obtiene el ID único del recurso.
     * @return Identificador único de 64 bits.
     */
    uint64_t
        GetID() const { return m_id; }

protected:
    std::string m_name;      ///< Nombre del recurso.
    std::string m_filePath;  ///< Ruta del archivo en disco.
    ResourceType m_type;     ///< Tipo de recurso.
    ResourceState m_state;   ///< Estado de carga.
    uint64_t m_id;           ///< Identificador único.

private:
    /**
     * @brief Genera un ID único para cada instancia de recurso.
     * @return Nuevo ID secuencial.
     */
    static uint64_t
        GenerateID() {
        static uint64_t nextID = 1;
        return nextID++;
    }
};