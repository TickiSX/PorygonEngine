#pragma once

#include "Prerequisites.h"

// Forward Declarations
// Usamos declaraciones adelantadas para evitar dependencias circulares y mejorar tiempo de compilación.
class Device;
class DeviceContext;

/**
 * @class SamplerState
 * @brief Encapsula un estado de muestreo (ID3D11SamplerState) de DirectX 11.
 *
 * El Sampler State define CÓMO la GPU lee los píxeles (texels) de una textura.
 * Controla dos aspectos fundamentales:
 * 1. **Filtrado:** Qué hacer cuando la textura se ve más grande (Magnification) o más pequeña (Minification) que su tamaño original (ej. Linear, Point, Anisotropic).
 * 2. **Direccionamiento (Addressing):** Qué hacer cuando las coordenadas UV salen del rango [0, 1] (ej. Wrap/Repetir, Clamp/Estirar, Mirror/Espejo).
 */
class SamplerState {
public:
    /**
     * @brief Constructor por defecto.
     */
    SamplerState() = default;

    /**
     * @brief Destructor por defecto.
     */
    ~SamplerState() = default;

    /**
     * @brief Crea e inicializa el estado de muestreo en la GPU.
     *
     * Generalmente configura un filtro lineal (para suavizado) y modo Wrap (repetición)
     * por defecto, a menos que se especifique lo contrario en la implementación.
     *
     * @param device Referencia al dispositivo encargado de crear el recurso.
     * @return HRESULT S_OK si la creación fue exitosa.
     */
    HRESULT init(Device& device);

    /**
     * @brief Actualiza la lógica del sampler (generalmente no necesario).
     */
    void update();

    /**
     * @brief Vincula el Sampler State al pipeline (generalmente al Pixel Shader).
     *
     * Permite que los shaders utilicen esta configuración para muestrear texturas.
     *
     * @param deviceContext Contexto del dispositivo.
     * @param StartSlot     Ranura inicial (s0, s1, etc.) donde se asignará el sampler.
     * @param NumSamplers   Número de samplers a asignar simultáneamente.
     */
    void render(DeviceContext& deviceContext,
        unsigned int StartSlot,
        unsigned int NumSamplers);

    /**
     * @brief Libera el recurso ID3D11SamplerState de la memoria.
     */
    void destroy();

public:
    /**
     * @brief Puntero nativo a la interfaz de estado de muestreo de DirectX.
     */
    ID3D11SamplerState* m_sampler = nullptr;
};