/**
 * @file MeshRendererComponent.h
 * @brief Implementa el componente de renderizado de mallas para el Entity Component System (ECS).
 * @ingroup ecs
 */
#pragma once
#include "Prerequisites.h"
#include "ECS/Component.h"

class Mesh;
class MaterialInstance;
class DeviceContext;

/**
 * @class MeshRendererComponent
 * @brief Componente encargado de vincular la geometría 3D con sus materiales correspondientes.
 * @details Provee al pipeline gráfico (como el ForwardRenderer) los datos necesarios para dibujar
 * un Actor en la escena. Soporta tanto mallas de un solo material como mallas complejas con
 * múltiples submallas (Submeshes) y múltiples instancias de materiales.
 */
class MeshRendererComponent : public Component {
public:
	/**
	 * @brief Constructor por defecto.
	 * Inicializa el componente especificando su tipo como MESH.
	 */
	MeshRendererComponent()
		: Component(ComponentType::MESH) {
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
	 * @param deviceContext Referencia al contexto del dispositivo.
	 */
	void render(DeviceContext& deviceContext) override {}

	/**
	 * @brief Libera los recursos del componente de forma segura (Vacío por defecto).
	 */
	void destroy() override {}

	/**
	 * @brief Asigna la malla geométrica 3D que este componente representará.
	 * @param mesh Puntero a la instancia de la malla (Mesh).
	 */
	void setMesh(Mesh* mesh) { m_mesh = mesh; }

	/**
	 * @brief Obtiene la malla 3D asociada al componente.
	 * @return Puntero a la malla geométrica.
	 */
	Mesh* getMesh() const { return m_mesh; }

	/**
	 * @brief Asigna una única instancia de material al componente.
	 * @details Este método limpia cualquier lista previa de materiales y establece
	 * el material proporcionado como el único (y principal) material del componente.
	 * @param materialInstance Puntero a la instancia del material.
	 */
	void setMaterialInstance(MaterialInstance* materialInstance) {
		m_materialInstance = materialInstance;
		m_materialInstances.clear();
		if (materialInstance) {
			m_materialInstances.push_back(materialInstance);
		}
	}

	/**
	 * @brief Obtiene el material principal del componente.
	 * @return Puntero al primer material o material principal.
	 */
	MaterialInstance* getMaterialInstance() const { return m_materialInstance; }

	/**
	 * @brief Asigna una lista de instancias de materiales, útil para mallas con múltiples submallas.
	 * @details Reemplaza la lista actual. Si la lista no está vacía, el primer elemento
	 * se convierte en el material principal.
	 * @param materialInstances Vector de punteros a instancias de materiales.
	 */
	void setMaterialInstances(const std::vector<MaterialInstance*>& materialInstances) {
		m_materialInstances = materialInstances;
		m_materialInstance = m_materialInstances.empty() ? nullptr : m_materialInstances.front();
	}

	/**
	 * @brief Añade un material adicional a la lista de materiales del componente.
	 * @details Útil para construir progresivamente los materiales de una malla compleja.
	 * @param materialInstance Puntero a la instancia de material a añadir.
	 */
	void addMaterialInstance(MaterialInstance* materialInstance) {
		if (!materialInstance) {
			return;
		}
		if (!m_materialInstance) {
			m_materialInstance = materialInstance;
		}
		m_materialInstances.push_back(materialInstance);
	}

	/**
	 * @brief Obtiene la lista completa de instancias de materiales asociadas al componente.
	 * @return Referencia constante al vector de materiales.
	 */
	const std::vector<MaterialInstance*>& getMaterialInstances() const { return m_materialInstances; }

	/**
	 * @brief Verifica si el componente debe ser renderizado en pantalla.
	 * @return true si es visible, false si está oculto.
	 */
	bool isVisible() const { return m_visible; }

	/**
	 * @brief Establece la visibilidad del componente.
	 * @param visible true para mostrar, false para ocultar en el renderizado.
	 */
	void setVisible(bool visible) { m_visible = visible; }

	/**
	 * @brief Verifica si la malla está configurada para proyectar sombras direccionales o de punto.
	 * @return true si genera sombras, false en caso contrario.
	 */
	bool canCastShadow() const { return m_castShadow; }

	/**
	 * @brief Establece si la malla debe ser evaluada en los pases de mapeo de sombras.
	 * @param value true para proyectar sombras, false para ignorarla en el shadow map.
	 */
	void setCastShadow(bool value) { m_castShadow = value; }

private:
	Mesh* m_mesh = nullptr;                                    /**< Puntero a la geometría 3D asociada. */
	MaterialInstance* m_materialInstance = nullptr;            /**< Puntero de acceso rápido al material principal (slot 0). */
	std::vector<MaterialInstance*> m_materialInstances;        /**< Colección de materiales vinculados a las submallas. */
	bool m_visible = true;                                     /**< Bandera lógica que determina si la malla entra a la cola de renderizado. */
	bool m_castShadow = true;                                  /**< Bandera lógica que permite o impide a la malla participar en el pase de sombras. */
};