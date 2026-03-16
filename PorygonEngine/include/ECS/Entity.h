#pragma once
#include "Prerequisites.h"
#include "Component.h"

// Forward Declaration
class DeviceContext;

/**
 * @class Entity
 * @brief Clase base abstracta para todas las entidades del juego.
 *
 * Una Entity representa un objeto en el mundo del juego. Actúa como un contenedor
 * para componentes y define el ciclo de vida básico (init, update, render, destroy).
 */
class
    Entity {
public:
    /**
     * @brief Constructor por defecto.
     */
    Entity() = default;

    /**
     * @brief Destructor virtual.
     */
    virtual
        ~Entity() = default;

    /**
     * @brief Método de activación inicial de la entidad.
     */
    virtual void
        awake() = 0;

    /**
     * @brief Inicializa la entidad.
     *
     * @note Método virtual puro. Debe ser implementado por las clases derivadas.
     */
    virtual void
        init() = 0;

    /**
     * @brief Actualiza la lógica de la entidad.
     *
     * @param deltaTime     El tiempo transcurrido (en segundos) desde la última actualización.
     * @param deviceContext Contexto del dispositivo para actualizaciones de subrecursos.
     */
    virtual void
        update(float deltaTime, DeviceContext& deviceContext) = 0;

    /**
     * @brief Renderiza la entidad.
     *
     * @param deviceContext Contexto del dispositivo para operaciones gráficas.
     */
    virtual void
        render(DeviceContext& deviceContext) = 0;

    /**
     * @brief Libera los recursos asociados a la entidad.
     *
     * Método virtual puro para la limpieza manual de memoria o recursos de API.
     */
    virtual void
        destroy() = 0;

    /**
     * @brief Agrega un componente a la entidad.
     *
     * @tparam T Tipo del componente, debe derivar de la clase @c Component.
     * @param component Puntero compartido (Shared Pointer) al componente que se va a agregar.
     */
    template <typename T>
    void
        addComponent(EU::TSharedPointer<T> component) {
        static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
        // Nota: Se usa .template porque dynamic_pointer_cast es un miembro plantilla dependiente
        m_components.push_back(component.template dynamic_pointer_cast<Component>());
    }

    /**
     * @brief Busca y obtiene un componente específico de la entidad.
     *
     * @tparam T Tipo del componente a obtener.
     * @return Puntero compartido al componente si se encuentra; puntero nulo/vacío en caso contrario.
     */
    template<typename T>
    EU::TSharedPointer<T>
        getComponent() {
        for (auto& component : m_components) {
            EU::TSharedPointer<T> specificComponent = component.template dynamic_pointer_cast<T>();
            if (specificComponent) {
                return specificComponent;
            }
        }
        return EU::TSharedPointer<T>();
    }

protected:
    /**
     * @brief Indica si la entidad está activa en la escena.
     */
    bool m_isActive = true;

    /**
     * @brief Identificador único de la entidad.
     */
    int m_id = -1;

    /**
     * @brief Lista de componentes adjuntos a esta entidad.
     */
    std::vector<EU::TSharedPointer<Component>> m_components;
};