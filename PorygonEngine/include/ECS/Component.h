#pragma once
#include "Prerequisites.h"

// Forward Declaration
class DeviceContext;

/**
 * @class Component
 * @brief Clase base abstracta para todos los componentes del juego.
 *
 * La clase Component define la interfaz básica que todos los componentes deben implementar,
 * permitiendo inicializar, actualizar, renderizar y destruir el componente de manera polimórfica.
 */
class
	Component {
public:
	/**
	 * @brief Constructor por defecto.
	 */
	Component() = default;

	/**
	 * @brief Constructor con asignación de tipo.
	 * @param type Identificador del tipo de componente.
	 */
	Component(const ComponentType type) : m_type(type) {}

	/**
	 * @brief Destructor virtual.
	 */
	virtual
		~Component() = default;

	/**
	 * @brief Inicializa el componente.
	 * * @note Método virtual puro. Debe ser implementado por la clase derivada para configurar recursos iniciales.
	 */
	virtual void
		init() = 0;

	/**
	 * @brief Actualiza la lógica del componente.
	 * * @param deltaTime El tiempo en segundos transcurrido desde la última actualización.
	 */
	virtual void
		update(float deltaTime) = 0;

	/**
	 * @brief Renderiza el componente.
	 * * @param deviceContext Contexto del dispositivo para emitir comandos gráficos.
	 */
	virtual void
		render(DeviceContext& deviceContext) = 0;

	/**
	 * @brief Libera los recursos asociados al componente.
	 * * @note Método virtual puro para limpieza manual de memoria o recursos de API.
	 */
	virtual void
		destroy() = 0;

	/**
	 * @brief Obtiene el tipo del componente.
	 * @return Enum que representa el tipo de componente actual.
	 */
	ComponentType
		getType() const { return m_type; }

protected:
	/**
	 * @brief Tipo del componente (identificador).
	 */
	ComponentType m_type;
};