// ModelLoader.cpp

#include "ModelLoader.h"
#include <fstream> 
#include <sstream> 
#include <vector>
#include <string>
#include <map>
#include <tuple> 

// #include "OBJ_Loader.h" <-- La dependencia ha sido ELIMINADA y reemplazada

// ----------------------------------------------------------------------------------
// Funciones Auxiliares para el Parsing (implementadas en un namespace anónimo)
// ----------------------------------------------------------------------------------
namespace {
    /**
     * @brief Parsea un token de cara (ej: "1/2/3") y extrae los índices v, vt, vn.
     * @param token String del vértice de la cara.
     * @return Una tupla de índices (v, vt, vn).
     */
    std::tuple<int, int, int> parseFaceIndex(const std::string& token) {
        int v = 0, vt = 0, vn = 0;
        std::stringstream ss(token);
        std::string segment;

        // v
        std::getline(ss, segment, '/');
        if (!segment.empty()) v = std::stoi(segment);

        // vt
        if (std::getline(ss, segment, '/')) {
            if (!segment.empty()) vt = std::stoi(segment);
        }

        // vn
        if (std::getline(ss, segment, '/')) {
            if (!segment.empty()) vn = std::stoi(segment);
        }

        return std::make_tuple(v, vt, vn);
    }
}

// ... [Aquí irían tus funciones init, update, render, destroy si las tienes] ...

// ----------------------------------------------------------------------------------
// Implementación del Parser Manual de OBJ (ModelLoader::Load)
// ----------------------------------------------------------------------------------
LoadData
ModelLoader::Load(const std::string& objFileName)
{
    LoadData LD;
    LD.name = objFileName;

    // 1. Estructuras temporales para datos RAW (Indices 1-basados, se añade un dummy en [0])
    std::vector<XMFLOAT3> temp_positions;
    std::vector<XMFLOAT2> temp_texCoords;
    std::vector<XMFLOAT3> temp_normals;

    // Añadir valores dummy para que los índices 1-basados del archivo OBJ apunten al elemento 1 de C++ (vector[1])
    temp_positions.push_back({ 0, 0, 0 });
    temp_texCoords.push_back({ 0, 0 });
    temp_normals.push_back({ 0, 0, 0 });

    // 2. Cache para generar el buffer indexado único
    std::map<VertexIndices, unsigned int> vertex_cache;
    unsigned int next_index = 0; // índice para el próximo SimpleVertex único

    std::ifstream file(objFileName);
    std::string line;

    if (!file.is_open()) {
        ERROR("ModelLoader", "Load", ("No se pudo abrir el archivo .obj: " + objFileName).c_str());
        return LD;
    }

    MESSAGE("ModelLoader", "Load", ("Iniciando parsing manual de: " + objFileName).c_str());

    // 3. Lectura línea por línea
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string token;
        ss >> token;

        if (token == "v") { // Posiciones
            XMFLOAT3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            temp_positions.push_back(pos);
        }
        else if (token == "vt") { // Coordenadas de textura
            XMFLOAT2 tc;
            ss >> tc.x >> tc.y;
            // Invertir la coordenada V (o Y) para la convención de DirectX/D3DX (V=0 arriba)
            tc.y = 1.0f - tc.y;
            temp_texCoords.push_back(tc);
        }
        else if (token == "vn") { // Normales
            XMFLOAT3 norm;
            ss >> norm.x >> norm.y >> norm.z;
            temp_normals.push_back(norm);
        }
        else if (token == "f") { // Caras y Triangulación
            std::vector<VertexIndices> face_vertices;
            std::string face_token;
            while (ss >> face_token) {
                face_vertices.push_back({
                    std::get<0>(parseFaceIndex(face_token)),
                    std::get<1>(parseFaceIndex(face_token)),
                    std::get<2>(parseFaceIndex(face_token))
                    });
            }

            // Triangulación "Fan" para N-gons (N > 3)
            if (face_vertices.size() < 3) continue;

            // Un N-gon se divide en N-2 triángulos, todos pivotando en el primer vértice (indice 0)
            for (size_t i = 0; i < face_vertices.size() - 2; ++i) {
                // Triángulo: [0], [i+1], [i+2]
                VertexIndices tri_indices[] = {
                    face_vertices[0],
                    face_vertices[i + 1],
                    face_vertices[i + 2]
                };

                // Procesar cada vértice del triángulo para indexación
                for (int j = 0; j < 3; ++j) {
                    const auto& current_key = tri_indices[j];

                    // Validación básica de índices (asume índices 1-basados positivos)
                    if (current_key.v <= 0 || current_key.v >= temp_positions.size() ||
                        (current_key.vt > 0 && current_key.vt >= temp_texCoords.size()) ||
                        (current_key.vn > 0 && current_key.vn >= temp_normals.size())) {
                        ERROR("ModelLoader", "Load", "Índice de vértice fuera de rango o inválido en cara.");
                        continue;
                    }

                    // 4. Indexación con Cache
                    auto it = vertex_cache.find(current_key);
                    if (it != vertex_cache.end()) {
                        // Vértice ya existe: reusar índice
                        LD.index.push_back(it->second);
                    }
                    else {
                        // Nuevo vértice: crear SimpleVertex
                        SimpleVertex new_vertex;
                        new_vertex.Pos = temp_positions[current_key.v];
                        // Solo asignar si el índice existe (0 si no se encuentra/es 0)
                        new_vertex.Tex = (current_key.vt > 0) ? temp_texCoords[current_key.vt] : XMFLOAT2(0, 0);
                        new_vertex.Normal = (current_key.vn > 0) ? temp_normals[current_key.vn] : XMFLOAT3(0, 0, 0);

                        // Añadir nuevo vértice y actualizar cache
                        LD.vertex.push_back(new_vertex);
                        vertex_cache[current_key] = next_index;

                        // Añadir índice al buffer de índices y avanzar el contador
                        LD.index.push_back(next_index);
                        next_index++;
                    }
                }
            }
        }
        // Se ignoran comandos como 'g', 'usemtl', 's', etc.
    }

    file.close();

    // 5. Finalización
    LD.numVertex = static_cast<int>(LD.vertex.size());
    LD.numIndex = static_cast<int>(LD.index.size());

    // --- CORRECCIÓN AQUÍ ---
    // Se añade .c_str() después de envolver la concatenación
    MESSAGE("ModelLoader", "Load", ("Parsing OBJ finalizado. Vertices unicos: " + std::to_string(LD.numVertex) +
        ", Indices: " + std::to_string(LD.numIndex)).c_str());

    return LD;
}