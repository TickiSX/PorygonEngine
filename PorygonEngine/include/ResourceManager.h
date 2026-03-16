#pragma once
#include "Prerequisites.h"
#include "IResource.h"

/**
 * @class ResourceManager
 * @brief Gestor centralizado de recursos (Singleton).
 *
 * Implementa el patrón Flyweight para evitar cargar duplicados del mismo recurso en memoria.
 * Almacena los recursos en un mapa hash usando una clave (string) y gestiona su ciclo de vida.
 */
class
    ResourceManager {
public:
    /**
     * @brief Constructor por defecto.
     */
    ResourceManager() = default;

    /**
     * @brief Destructor por defecto.
     */
    ~ResourceManager() = default;

    /**
     * @brief Acceso a la instancia única del Singleton.
     * @return Referencia a la instancia estática del @c ResourceManager.
     */
    static ResourceManager&
        getInstance() {
        static ResourceManager instance;
        return instance;
    }

    // Borramos constructores de copia y asignación para garantizar unicidad.
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    /**
     * @brief Obtiene un recurso existente o lo carga si no está en caché.
     *
     * Este es el método principal para solicitar recursos. Si el recurso ya existe (buscado por key),
     * devuelve un puntero compartido al existente. Si no, crea una nueva instancia de tipo T,
     * la carga desde el archivo y la inicializa.
     *
     * @tparam T    Tipo del recurso (debe heredar de @c IResource).
     * @tparam Args Argumentos variádicos para el constructor del recurso.
     *
     * @param key      Identificador único para el recurso en el caché.
     * @param filename Ruta del archivo en disco para cargar.
     * @param args     Argumentos adicionales para el constructor de T.
     *
     * @return @c std::shared_ptr<T> al recurso listo para usar, o @c nullptr si falló la carga.
     */
    template<typename T, typename... Args>
    std::shared_ptr<T>
        GetOrLoad(const std::string& key, const std::string& filename, Args&&... args) {
        static_assert(std::is_base_of<IResource, T>::value, "T debe heredar de IResource");

        // 1. ¿Ya existe el recurso en el caché?
        auto it = m_resources.find(key);
        if (it != m_resources.end()) {
            // Intentar castear al tipo correcto
            auto existing = std::dynamic_pointer_cast<T>(it->second);
            if (existing && existing->GetState() == ResourceState::Loaded) {
                return existing; // Flyweight: reutilizamos la instancia existente
            }
        }

        // 2. No existe o no está cargado -> crearlo y cargarlo
        std::shared_ptr<T> resource = std::make_shared<T>(key, std::forward<Args>(args)...);

        if (!resource->load(filename)) {
            // Hubo un error al cargar el archivo
            return nullptr;
        }

        if (!resource->init()) {
            // Hubo un error al inicializar en API gráfica
            return nullptr;
        }

        // 3. Guardar en el caché y devolver
        m_resources[key] = resource;
        return resource;
    }

    /**
     * @brief Obtiene un recurso ya cargado sin intentar cargarlo si no existe.
     *
     * @tparam T Tipo al que se desea castear el recurso.
     * @param key Clave del recurso.
     * @return @c std::shared_ptr<T> si existe, @c nullptr si no se encuentra.
     */
    template<typename T>
    std::shared_ptr<T>
        Get(const std::string& key) const {
        auto it = m_resources.find(key);
        if (it == m_resources.end()) {
            return nullptr;
        }

        return std::dynamic_pointer_cast<T>(it->second);
    }

    /**
     * @brief Libera un recurso específico de la memoria.
     *
     * Llama a @c unload() en el recurso y lo elimina del mapa de gestión.
     * @param key Clave del recurso a eliminar.
     */
    void
        Unload(const std::string& key) {
        auto it = m_resources.find(key);
        if (it != m_resources.end()) {
            it->second->unload();
            m_resources.erase(it);
        }
    }

    /**
     * @brief Libera todos los recursos gestionados.
     *
     * Útil al cerrar la aplicación o cambiar de nivel/escena.
     */
    void
        UnloadAll() {
        for (auto& [key, res] : m_resources) {
            if (res) {
                res->unload();
            }
        }
        m_resources.clear();
    }

private:
    /**
     * @brief Mapa hash que almacena los recursos cargados.
     * @details Clave: string (ID/Nombre), Valor: Shared Pointer al recurso base.
     */
    std::unordered_map<std::string, std::shared_ptr<IResource>> m_resources;
};