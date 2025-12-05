#pragma once

#include "Prerequisites.h"
#include <vector>

// Forward Declarations
class Device;
class DeviceContext;

/**
 * @class InputLayout
 * @brief Define la estructura de los datos de vértices para el pipeline gráfico.
 *
 * El Input Layout es un objeto de DirectX que actúa como un "mapa" o "instrucciones de decodificación".
 * Le dice a la GPU cómo interpretar los bits crudos almacenados en el Vertex Buffer
 * (Posición, Normales, UVs, Color) y cómo mapearlos a las variables de entrada del Vertex Shader.
 */
class InputLayout {
public:
    /**
     * @brief Constructor por defecto.
     */
    InputLayout() = default;

    /**
     * @brief Destructor por defecto.
     */
    ~InputLayout() = default;

    /**
     * @brief Inicializa y crea el Input Layout en la GPU.
     *
     * Valida que la descripción de los elementos coincida con la firma de entrada
     * del Vertex Shader compilado.
     *
     * @param device           Referencia al dispositivo para crear el recurso.
     * @param Layout           Vector de descriptores (D3D11_INPUT_ELEMENT_DESC) que definen el formato (ej. Position, TexCoord).
     * @param VertexShaderData Blob binario del Vertex Shader compilado (necesario para la validación).
     * @return HRESULT         S_OK si la creación fue exitosa.
     */
    HRESULT init(Device& device,
        std::vector<D3D11_INPUT_ELEMENT_DESC>& Layout,
        ID3DBlob* VertexShaderData);

    /**
     * @brief Actualiza la lógica del layout.
     * @note Generalmente no se requiere actualización para este recurso estático.
     */
    void update();

    /**
     * @brief Vincula el Input Layout al pipeline (Etapa Input Assembler).
     *
     * Debe llamarse antes de dibujar la malla para que la GPU sepa cómo leer los vértices.
     *
     * @param deviceContext Contexto del dispositivo.
     */
    void render(DeviceContext& deviceContext);

    /**
     * @brief Libera el recurso ID3D11InputLayout.
     */
    void destroy();

public:
    /**
     * @brief Puntero nativo al recurso de DirectX.
     */
    ID3D11InputLayout* m_inputLayout = nullptr;
};