// ModelLoader.h

#pragma once
#include "Prerequisites.h"
#include <fstream> // Necesario para lectura de archivos
#include <sstream> // Necesario para parseo de strings
#include <vector>
#include <string>
#include <map>
#include <tuple> // Opcional, pero util para comparaciones

/**
 * @class ModelLoader
 * @brief Clase encargada de gestionar la carga de modelos 3D con un parser OBJ manual.
 */
class
	ModelLoader
{
public:
	// ... [Constructor, Destructor, init, update, render, destroy] ...

	/**
	 * @brief Carga un archivo de modelo 3D (formato OBJ) usando el parser manual.
	 * @param objFileName Nombre o ruta del archivo OBJ a cargar.
	 * @return Estructura LoadData que contiene los datos del modelo cargado.
	 */
	LoadData
		Load(const std::string& objFileName);

private:
	/**
	 * @brief Estructura auxiliar que almacena los ndices de Posicin (v),
	 * Coordenadas de Textura (vt) y Normal (vn) de un vrtice OBJ.
	 * Se usa como clave para el cache de vrtices nicos.
	 */
	struct VertexIndices {
		int v, vt, vn;
		// Operador de comparacin para usar como clave en std::map
		bool operator<(const VertexIndices& other) const {
			if (v != other.v) return v < other.v;
			if (vt != other.vt) return vt < other.vt;
			return vn < other.vn;
		}
	};
};