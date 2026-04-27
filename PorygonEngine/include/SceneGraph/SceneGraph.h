/**
 * @file SceneGraph.h
 * @brief Declara el gestor de la jerarquía espacial e instanciación del motor.
 * @ingroup core
 */
#pragma once
#include "Prerequisites.h"

class Entity;
class DeviceContext;
class Camera;
class RenderScene;

/**
 * @class SceneGraph
 * @brief Estructura de árbol que administra las relaciones espaciales y lógicas de las entidades.
 * @details El SceneGraph es responsable de mantener la jerarquía de transformaciones (Padre-Hijo).
 * Calcula las matrices de mundo (World Matrices) de forma recursiva, asegurando que las transformaciones
 * locales de un hijo se multipliquen correctamente por la posición, rotación y escala de su padre.
 * Además, actúa como el puente principal entre el Entity Component System (ECS) y el RenderPipeline,
 * recopilando los componentes visuales en una estructura plana (`RenderScene`) para su dibujado.
 */
class SceneGraph {
public:
	/**
	 * @brief Constructor por defecto.
	 */
	SceneGraph() = default;

	/**
	 * @brief Destructor por defecto.
	 */
	~SceneGraph() = default;

	/**
	 * @brief Inicializa las estructuras internas del grafo de escena.
	 */
	void init();

	/**
	 * @brief Registra una nueva entidad dentro del grafo.
	 * @details Al añadirse, la entidad se considera un nodo raíz (Root) sin padre asignado.
	 * @param e Puntero a la entidad a registrar.
	 */
	void addEntity(Entity* e);

	/**
	 * @brief Elimina una entidad del grafo de escena.
	 * @details Si la entidad tiene hijos, la jerarquía debe ser gestionada para evitar punteros colgantes.
	 * @param e Puntero a la entidad a remover.
	 */
	void removeEntity(Entity* e);

	/**
	 * @brief Verifica si un nodo específico es ancestro (padre, abuelo, etc.) de otro nodo.
	 * @details Esta comprobación es crítica para evitar referencias circulares (ciclos infinitos)
	 * al momento de emparentar entidades.
	 * @param possibleAncestor La entidad que podría estar en un nivel superior jerárquico.
	 * @param node La entidad objetivo a evaluar.
	 * @return true si `possibleAncestor` está en la cadena jerárquica ascendente de `node`.
	 */
	bool isAncestor(Entity* possibleAncestor, Entity* node) const;

	/**
	 * @brief Vincula una entidad como hija de otra entidad.
	 * @details El hijo heredará las transformaciones espaciales globales del padre.
	 * La función fallará si se intenta crear una relación circular.
	 * @param child La entidad subordinada.
	 * @param parent La entidad dominante.
	 * @return true si el emparentamiento fue exitoso, false si falló (ej. ciclo detectado).
	 */
	bool attach(Entity* child, Entity* parent);

	/**
	 * @brief Desvincula una entidad de su padre actual.
	 * @details La entidad volverá a ser un nodo raíz (Root) y su matriz de transformación
	 * pasará a ser global e independiente.
	 * @param child La entidad a desvincular.
	 * @return true si se desvinculó correctamente, false en caso contrario.
	 */
	bool detach(Entity* child);

	/**
	 * @brief Actualiza la lógica de todas las entidades y recalcula las matrices espaciales.
	 * @details Llama al método update de cada entidad y posteriormente ejecuta el cálculo
	 * recursivo de las transformaciones globales de arriba hacia abajo en la jerarquía.
	 * @param deltaTime Tiempo transcurrido desde el último frame.
	 * @param deviceContext Referencia al contexto del dispositivo.
	 */
	void update(float deltaTime, DeviceContext& deviceContext);

	/**
	 * @brief Ejecuta llamadas de dibujado directo de las entidades (Uso dependiente del pipeline).
	 * @param deviceContext Referencia al contexto del dispositivo.
	 */
	void render(DeviceContext& deviceContext);

	/**
	 * @brief Recopila la información visual de los nodos y la extrae a una estructura plana.
	 * @details Itera sobre todas las entidades registradas, evalúa su visibilidad (Culling)
	 * respecto a la cámara, e inserta sus mallas y luces en las colas correspondientes
	 * de la estructura `RenderScene` para que el `ISceneRenderer` las dibuje.
	 * @param outScene Estructura de salida donde se depositarán los datos de renderizado.
	 * @param camera La cámara desde cuya perspectiva se evaluará la escena.
	 */
	void gatherRenderScene(RenderScene& outScene, const Camera& camera);

	/**
	 * @brief Limpia el grafo de escena y libera las referencias a las entidades.
	 */
	void destroy();

private:
	/**
	 * @brief Calcula y propaga las transformaciones globales hacia los hijos.
	 * @param node El nodo actual siendo evaluado.
	 * @param parentWorld La matriz de transformación global de su padre directo.
	 */
	void updateWorldRecursive(Entity* node, const XMMATRIX& parentWorld);

	/**
	 * @brief Comprueba si una entidad se encuentra en la base jerárquica (sin padres).
	 * @param e La entidad a evaluar.
	 * @return true si no tiene padre, false en caso contrario.
	 */
	bool isRoot(Entity* e) const;

	/**
	 * @brief Verifica si el puntero de la entidad ya está siendo administrado por este grafo.
	 * @param e La entidad a evaluar.
	 * @return true si ya existe en la colección, false de lo contrario.
	 */
	bool isRegistered(Entity* e) const;

public:
	// TODO: Migrar a punteros inteligentes (std::shared_ptr) para evitar fugas de memoria si la entidad es destruida externamente.
	// std::vector<EU::TSharedPointer<Entity>> m_entities;

	std::vector<Entity*> m_entities; /**< Colección plana de todas las entidades registradas en el grafo. */
};