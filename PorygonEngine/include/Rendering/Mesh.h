/**
 * @file Mesh.h
 * @brief Declara las estructuras de datos para la representación de geometría 3D.
 * @ingroup rendering
 */
#pragma once
#include "Prerequisites.h"
#include "Buffer.h"

 /**
  * @struct Submesh
  * @brief Representa una subdivisión lógica de una malla 3D completa.
  * @details Los modelos 3D complejos suelen estar divididos en varias partes para poder
  * aplicarles diferentes materiales (por ejemplo, el metal del cañón y la madera de la
  * culata en un rifle). El `Submesh` encapsula la geometría de una de estas partes y
  * define exactamente qué sección de los buffers debe enviarse a la tarjeta gráfica
  * en una sola llamada de dibujado (Draw Call).
  */
struct Submesh {
	Buffer vertexBuffer;       /**< Buffer en la memoria de la GPU que contiene los datos de los vértices (Posición, Normales, UVs, etc.). */
	Buffer indexBuffer;        /**< Buffer en la memoria de la GPU que define el orden de los vértices para formar triángulos. */

	unsigned int indexCount = 0;   /**< Cantidad total de índices que componen esta submalla. */
	unsigned int startIndex = 0;   /**< Desplazamiento (offset) en el Index Buffer desde donde se debe empezar a leer. */
	unsigned int materialSlot = 0; /**< Índice que vincula esta submalla con un MaterialInstance específico en el MeshRendererComponent. */
};

/**
 * @class Mesh
 * @brief Representa un modelo 3D completo cargado en memoria.
 * @details Actúa como un contenedor de alto nivel que agrupa una o múltiples submallas (`Submesh`).
 * El motor utiliza esta clase para mantener organizada la geometría importada (por ejemplo, desde
 * un archivo .fbx o .obj) antes de que el `ForwardRenderer` la itere y la envíe al pipeline gráfico.
 */
class Mesh {
public:
	// =========================================================================
	// GETTERS
	// =========================================================================

	/**
	 * @brief Obtiene una referencia modificable a la lista de submallas.
	 * @return Referencia al vector que contiene los objetos Submesh.
	 */
	std::vector<Submesh>& getSubmeshes() { return m_submeshes; }

	/**
	 * @brief Obtiene una referencia de solo lectura a la lista de submallas.
	 * @return Referencia constante al vector de objetos Submesh.
	 */
	const std::vector<Submesh>& getSubmeshes() const { return m_submeshes; }

	// =========================================================================
	// GESTIÓN DE MEMORIA
	// =========================================================================

	/**
	 * @brief Libera los recursos de hardware de todas las submallas.
	 * @details Itera sobre cada `Submesh` destruyendo sus respectivos `vertexBuffer` e
	 * `indexBuffer`, liberando la VRAM en la tarjeta gráfica. Finalmente, limpia el vector interno.
	 * Debe llamarse antes de que el objeto Mesh sea destruido para evitar fugas de memoria (Memory Leaks).
	 */
	void destroy() {
		for (Submesh& submesh : m_submeshes) {
			submesh.vertexBuffer.destroy();
			submesh.indexBuffer.destroy();
		}
		m_submeshes.clear();
	}

private:
	std::vector<Submesh> m_submeshes; /**< Colección de particiones geométricas que conforman este modelo. */
};