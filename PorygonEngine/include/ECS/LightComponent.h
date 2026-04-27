/**
 * @file LightComponent.h
 * @brief Implementa el componente de luz para el Entity Component System (ECS).
 * @ingroup ecs
 */
#pragma once
#include "Prerequisites.h"
#include "ECS/Component.h"
#include "Rendering/RenderTypes.h"

class DeviceContext;

/**
 * @class LightComponent
 * @brief Componente que otorga capacidades de iluminación a un Actor en la escena.
 * * @details Este componente actúa principalmente como un contenedor de datos (Data Container)
 * para el sistema de renderizado. Almacena las propiedades físicas de la luz (como color,
 * intensidad y tipo) a través de la estructura LightData, y define si la luz debe
 * proyectar sombras en el ForwardRenderer.
 */
class LightComponent : public Component {
public:
	/**
	 * @brief Constructor por defecto.
	 * Inicializa el componente base.
	 */
	LightComponent()
		: Component(ComponentType::NONE) {
	}

	/**
	 * @brief Inicializa los recursos del componente (Vacío por defecto).
	 */
	void init() override {}

	/**
	 * @brief Actualiza la lógica del componente frame a frame (Vacío por defecto).
	 * @param deltaTime Tiempo transcurrido desde el último frame en segundos.
	 */
	void update(float deltaTime) override {}

	/**
	 * @brief Dibuja o procesa datos visuales específicos del componente (Vacío por defecto).
	 * @param deviceContext Referencia al contexto del dispositivo de DirectX 11.
	 */
	void render(DeviceContext& deviceContext) override {}

	/**
	 * @brief Libera los recursos del componente de forma segura (Vacío por defecto).
	 */
	void destroy() override {}

	/**
	 * @brief Obtiene una referencia modificable a los datos estructurales de la luz.
	 * @return Referencia a la estructura LightData (permite alterar color, intensidad, etc.).
	 */
	LightData& getLightData() { return m_light; }

	/**
	 * @brief Obtiene una referencia de solo lectura a los datos estructurales de la luz.
	 * @return Referencia constante a la estructura LightData.
	 */
	const LightData& getLightData() const { return m_light; }

	/**
	 * @brief Establece si esta luz debe procesarse en el pase de sombras (Shadow Mapping).
	 * @param value true para habilitar el casteo de sombras, false para deshabilitarlo.
	 */
	void setCastShadow(bool value) { m_castShadow = value; }

	/**
	 * @brief Verifica si la luz está configurada para proyectar sombras.
	 * @return true si el casteo de sombras está habilitado, false en caso contrario.
	 */
	bool canCastShadow() const { return m_castShadow; }

private:
	LightData m_light;          /**< Estructura base que contiene las propiedades fundamentales de la luz (color, tipo, rango, etc.). */
	bool m_castShadow = false;  /**< Bandera que indica si la luz genera mapa de sombras. Apagada por defecto por optimización. */
};