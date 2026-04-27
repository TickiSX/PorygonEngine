/**
 * @file RenderScene.h
 * @brief Declara la estructura de datos que representa una escena lista para ser dibujada.
 * @ingroup rendering
 */
#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

class Skybox;

/**
 * @class RenderScene
 * @brief Contenedor lógico que agrupa todos los elementos visuales activos en el frame actual.
 * @details Esta clase actúa como un "Snapshot" (captura) del estado del juego.
 * En cada fotograma, el Entity Component System (ECS) o el Scene Manager extrae la
 * información geométrica y lumínica de los actores visibles y puebla esta estructura.
 * Gracias a esto, el pipeline gráfico (Forward o Deferred) no necesita saber qué es un "Actor"
 * o un "Componente", simplemente itera sobre estas listas planas de datos puros y los dibuja.
 */
class RenderScene {
public:
	/**
	 * @brief Vacia todas las listas y reinicia los punteros de la escena actual.
	 * @details Esta función es crítica y debe llamarse al comienzo del ciclo de dibujado
	 * de cada frame, justo antes de que el motor empiece a recolectar y poblar los
	 * objetos que pasaron la prueba de Frustum Culling.
	 */
	void clear();

public:
	// =========================================================================
	// COLAS DE GEOMETRÍA
	// =========================================================================

	/** * @brief Lista plana de objetos sólidos (Depth-Write habilitado).
	 * El renderer suele agruparlos y dibujarlos de frente hacia atrás (Front-to-Back) para aprovechar el Early-Z.
	 */
	std::vector<RenderObject> opaqueObjects;

	/** * @brief Lista plana de objetos con mezcla de colores (Alpha, Aditivo, etc.).
	 * El renderer debe ordenarlos y dibujarlos estrictamente de atrás hacia adelante (Back-to-Front).
	 */
	std::vector<RenderObject> transparentObjects;

	// =========================================================================
	// ILUMINACIÓN Y ENTORNO
	// =========================================================================

	/** * @brief Lista de luces direccionales activas (ej. el Sol o luz principal).
	 * Típicamente, el motor toma el primer elemento de esta lista para generar el mapa de sombras principal.
	 */
	std::vector<LightData> directionalLights;

	/** * @brief Puntero al entorno cúbico de fondo.
	 * Si no es nulo, el renderer dibujará este cielo cubriendo todos los fragmentos vacíos de la pantalla.
	 */
	Skybox* skybox = nullptr;
};