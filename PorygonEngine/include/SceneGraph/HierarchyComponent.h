#pragma once
#include "Prerequisites.h"
#include "ECS/Component.h"

class DeviceContext;
class Entity;

/**
 * @class HierarchyComponent
 * @brief Componente responsable de gestionar las relaciones de jerarquía (padre-hijo) entre entidades.
 *
 * Permite que las entidades se organicen en una estructura de árbol (Grafo de Escena),
 * facilitando transformaciones relativas y propagación de estados.
 */
class
    HierarchyComponent : public Component {
public:
    /**
     * @brief Constructor por defecto.
     * Asigna el tipo de componente como @c HIERARCHY.
     */
    HierarchyComponent()
        : Component(ComponentType::HIERARCHY) {
    }

    /**
     * @brief Destructor por defecto.
     */
    ~HierarchyComponent() = default;

    /**
     * @brief Inicialización del componente.
     * @note Actualmente no realiza ninguna operación.
     */
    void
        init() override {}

    /**
     * @brief Actualización por frame.
     * @param delta_time Tiempo transcurrido desde el último frame.
     */
    void
        update(float delta_time) override {}

    /**
     * @brief Renderizado del componente.
     * @param deviceContext Contexto del dispositivo gráfico.
     */
    void
        render(DeviceContext& deviceContext) override {}

    /**
     * @brief Limpieza de recursos al destruir el componente.
     *
     * Elimina las referencias a los hijos y desconecta al padre para evitar
     * punteros colgantes dentro de la lógica de este componente.
     */
    void
        destroy() override {
        m_children.clear();
        m_parent = nullptr;
    }

    // =========================================================
    // API SceneGraph (Gestión de grafo de escena)
    // =========================================================

    /**
     * @brief Establece la entidad padre de esta entidad.
     * @param parent Puntero a la entidad que actuará como padre.
     */
    void
        setParent(Entity* parent) {
        m_parent = parent;
    }

    /**
     * @brief Verifica si esta entidad es una raíz (no tiene padre).
     * @return @c true si @c m_parent es @c nullptr, @c false en caso contrario.
     */
    bool
        isRoot() const {
        return m_parent == nullptr;
    }

    /**
     * @brief Verifica si la entidad tiene hijos asociados.
     * @return @c true si la lista de hijos no está vacía.
     */
    bool
        hasChildren() const {
        return !m_children.empty();
    }

    /**
     * @brief Añade una entidad hija a la lista.
     *
     * Verifica primero si el hijo es válido y si ya existe en la lista
     * para evitar duplicados.
     *
     * @param child Puntero a la entidad a añadir.
     */
    void
        addChild(Entity* child) {
        if (!child) {
            return;
        }

        // Evitar añadir el mismo hijo dos veces
        if (std::find(m_children.begin(), m_children.end(), child) != m_children.end()) {
            return;
        }
        m_children.push_back(child);
    }

    /**
     * @brief Elimina una entidad hija de la lista.
     *
     * Utiliza el idioma Erase-Remove para eliminar eficientemente el elemento del vector.
     *
     * @param child Puntero a la entidad a remover.
     */
    void
        removeChild(Entity* child) {
        if (!child) return;

        m_children.erase(
            std::remove(m_children.begin(), m_children.end(), child),
            m_children.end()
        );
    }

public:
    /** * @brief Puntero a la entidad padre.
     * @details @c nullptr si la entidad es raíz.
     */
    Entity* m_parent = nullptr;

    /** * @brief Lista de punteros a las entidades hijas.
     */
    std::vector<Entity*> m_children;
};