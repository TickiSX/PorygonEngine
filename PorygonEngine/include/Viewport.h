#pragma once

#include "Prerequisites.h"

// Forward Declarations
class Window;
class DeviceContext;

/**
 * @class Viewport
 * @brief Define el área de la ventana donde se renderizará la imagen final.
 *
 * El Viewport (Puerto de Vista) es responsable de la transformación final en el pipeline
 * de rasterización. Convierte las coordenadas normalizadas del dispositivo (NDC, de -1 a 1)
 * en coordenadas de píxeles reales (Screen Space, de 0 a Ancho/Alto) y escala los valores de profundidad.
 */
class Viewport {
public:
    /**
     * @brief Constructor por defecto.
     * Inicializa la estructura interna a cero.
     */
    Viewport() = default;

    /**
     * @brief Destructor por defecto.
     */
    ~Viewport() = default;

    /**
     * @brief Inicializa el viewport basándose en el tamaño de una ventana.
     *
     * Configura el viewport para cubrir toda el área cliente de la ventana proporcionada.
     * Profundidad configurada en el rango estándar [0.0, 1.0].
     *
     * @param window Referencia a la ventana de la aplicación.
     * @return HRESULT S_OK si la operación fue exitosa.
     */
    HRESULT init(const Window& window);

    /**
     * @brief Inicializa el viewport con dimensiones específicas.
     *
     * Útil si se desea renderizar en solo una parte de la ventana (ej. pantalla dividida)
     * o en una textura fuera de pantalla con tamaño fijo.
     *
     * @param width  Ancho del área de renderizado en píxeles.
     * @param height Alto del área de renderizado en píxeles.
     * @return HRESULT S_OK si la operación fue exitosa.
     */
    HRESULT init(unsigned int width, unsigned int height);

    /**
     * @brief Actualiza la lógica del viewport.
     *
     * Generalmente no hace nada, a menos que el viewport sea dinámico (ej. animado).
     */
    void update();

    /**
     * @brief Vincula el viewport al Rasterizador (RS Stage).
     *
     * Le dice a la GPU en qué parte del Render Target debe dibujar los píxeles.
     *
     * @param deviceContext Contexto del dispositivo para ejecutar el comando RSSetViewports.
     */
    void render(DeviceContext& deviceContext);

    /**
     * @brief Libera recursos.
     *
     * Dado que D3D11_VIEWPORT es una estructura simple (POD) y no un puntero COM,
     * no hay memoria dinámica que liberar aquí.
     */
    void destroy() {}

public:
    /**
     * @brief Estructura nativa de DirectX que contiene TopLeftX, TopLeftY, Width, Height, MinDepth y MaxDepth.
     */
    D3D11_VIEWPORT m_viewport;
};