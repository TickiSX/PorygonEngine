#pragma once
#include "Prerequisites.h"

class Entity;
class DeviceContext;

/**
 * @class SceneGraph
 * @brief Gestiona la jerarquía global de la escena y el ciclo de vida de las entidades.
 * * Esta clase es responsable de:
 * 1. Mantener el registro de todas las entidades activas.
 * 2. Gestionar las relaciones Padre-Hijo (Attach/Detach).
 * 3. Propagar las transformaciones (matrices de mundo) desde los padres a los hijos.
 * 4. Orquestar el Update y Render de todas las entidades.
 */
class SceneGraph {
public:
	SceneGraph() = default;
	~SceneGraph() = default;

	/**
	 * @brief Inicializa el grafo de escena.
	 */
	void
		init();

	/**
	 * @brief Registra una nueva entidad en el sistema.
	 * * Añade la entidad a la lista plana de entidades gestionadas (`m_entities`).
	 * @param e Puntero a la entidad a registrar.
	 */
	void
		addEntity(Entity* e);

	/**
	 * @brief Elimina una entidad del sistema.
	 * * Debería encargarse también de limpiar relaciones (si la entidad tiene padres o hijos)
	 * antes de eliminarla de la lista.
	 * @param e Puntero a la entidad a eliminar.
	 */
	void
		removeEntity(Entity* e);

	/**
	 * @brief Verifica si una entidad es ancestro de otra.
	 * * Útil para evitar ciclos (ej: que el padre intente ser hijo de su propio hijo).
	 * @param possibleAncestor La entidad que sospechamos que está arriba en la jerarquía.
	 * @param node La entidad actual.
	 * @return true si 'possibleAncestor' es padre, abuelo, etc., de 'node'.
	 */
	bool
		isAncestor(Entity* possibleAncestor, Entity* node) const;

	/**
	 * @brief Vincula un hijo a un padre en la jerarquía.
	 * * Realiza validaciones como: evitar que sea null, evitar ciclos (usando isAncestor)
	 * y actualizar los componentes de jerarquía de ambas entidades.
	 * @param child La entidad que será movida.
	 * @param parent La entidad que actuará como contenedor.
	 * @return true si la vinculación fue exitosa, false si hubo un error (ej: ciclo detectado).
	 */
	bool
		attach(Entity* child, Entity* parent);

	/**
	 * @brief Desvincula una entidad de su padre.
	 * * La entidad 'child' se convierte en una entidad raíz (root) en el grafo.
	 * @param child La entidad a desvincular.
	 * @return true si se desvinculó correctamente.
	 */
	bool
		detach(Entity* child);

	/**
	 * @brief Actualiza la lógica de todas las entidades y recalcula las matrices de mundo.
	 * @param deltaTime Tiempo transcurrido desde el último frame.
	 * @param deviceContext Contexto del dispositivo para operaciones gráficas si son necesarias.
	 */
	void
		update(float deltaTime, DeviceContext& deviceContext);

	/**
	 * @brief Ejecuta la rutina de renderizado para todas las entidades.
	 * @param deviceContext Contexto del dispositivo gráfico necesario para dibujar.
	 */
	void
		render(DeviceContext& deviceContext);

	/**
	 * @brief Libera todos los recursos y elimina todas las entidades del grafo.
	 */
	void
		destroy();

private:
	/**
	 * @brief Método recursivo para actualizar las transformaciones mundiales.
	 * * Multiplica la matriz local de la entidad por la matriz mundial del padre
	 * (World = Local * ParentWorld) y propaga el resultado a los hijos.
	 * @param node Entidad actual que se está procesando.
	 * @param parentWorld Matriz de transformación mundial acumulada del padre.
	 */
	void
		updateWorldRecursive(Entity* node, const XMMATRIX& parentWorld);

	/**
	 * @brief Helper para comprobar si una entidad no tiene padre.
	 */
	bool
		isRoot(Entity* e) const;

	/**
	 * @brief Helper para verificar si una entidad ya existe en `m_entities`.
	 */
	bool
		isRegistered(Entity* e) const;

private:
	//std::vector<EU::TSharedPointer<Entity>> m_entities; // (Comentado en original)

public:
	/** * @brief Lista plana de todas las entidades en la escena.
	 * Nota: Es pública para acceso rápido, pero idealmente debería gestionarse vía getters/iteradores.
	 */
	std::vector<Entity*> m_entities;
};