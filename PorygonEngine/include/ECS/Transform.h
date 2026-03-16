#pragma once
#include "Prerequisites.h"
#include "EngineUtilities/Vectors/Vector3.h"
#include "Component.h"

/**
 * @class Transform
 * @brief Componente que define la posición, rotación y escala de una entidad en el espacio 3D.
 *
 * Este componente es responsable de calcular la Matriz de Mundo (World Matrix) combinando
 * las transformaciones de escala, rotación y traslación. Esta matriz es esencial para
 * el pipeline de renderizado.
 */
class
    Transform : public Component {
public:
    /**
     * @brief Constructor por defecto.
     * Inicializa los vectores en cero y asigna el tipo de componente.
     */
    Transform()
        : position(),
        rotation(),
        scale(),
        matrix(),
        Component(ComponentType::TRANSFORM) {
    }

    /**
     * @brief Inicializa los valores por defecto del transform.
     * Establece la escala a (1,1,1) y la matriz a la Identidad.
     */
    void
        init() override {
        scale.one(); // Asegura que la escala inicial no sea cero.
        matrix = XMMatrixIdentity();
    }

    /**
     * @brief Actualiza la matriz de transformación.
     *
     * Calcula la matriz final combinando: Escala * Rotación * Traslación (SRT).
     *
     * @param deltaTime Tiempo transcurrido desde la última actualización.
     */
    void
        update(float deltaTime) override {
        // 1. Matriz de Escala
        XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);

        // 2. Matriz de Rotación (Roll, Pitch, Yaw)
        XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

        // 3. Matriz de Traslación
        XMMATRIX translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);

        // 4. Composición: Scale -> Rotation -> Translation
        matrix = scaleMatrix * rotationMatrix * translationMatrix;
    }

    /**
     * @brief Renderiza el componente (sin implementación para Transform).
     * @param deviceContext Contexto del dispositivo.
     */
    void
        render(DeviceContext& deviceContext) override {}

    /**
     * @brief Libera recursos (sin implementación específica).
     */
    void
        destroy() override {}

    // ------------------------------------------------------------------------
    // Getters y Setters - Posición
    // ------------------------------------------------------------------------

    /**
     * @brief Obtiene la posición actual.
     * @return Referencia constante al vector de posición.
     */
    const EU::Vector3&
        getPosition() const { return position; }

    /**
     * @brief Establece una nueva posición.
     * @param newPos Nuevo vector de posición.
     */
    void
        setPosition(const EU::Vector3& newPos) { position = newPos; }

    /**
     * @brief Desplaza la posición actual sumando un vector.
     * @param translation Vector de desplazamiento (delta).
     */
    void
        translate(const EU::Vector3& translation);

    // ------------------------------------------------------------------------
    // Getters y Setters - Rotación
    // ------------------------------------------------------------------------

    /**
     * @brief Obtiene la rotación actual (en radianes o grados, según implementación del motor).
     * @return Referencia constante al vector de rotación.
     */
    const EU::Vector3&
        getRotation() const { return rotation; }

    /**
     * @brief Establece una nueva rotación.
     * @param newRot Nuevo vector de rotación.
     */
    void
        setRotation(const EU::Vector3& newRot) { rotation = newRot; }

    // ------------------------------------------------------------------------
    // Getters y Setters - Escala
    // ------------------------------------------------------------------------

    /**
     * @brief Obtiene la escala actual.
     * @return Referencia constante al vector de escala.
     */
    const EU::Vector3&
        getScale() const { return scale; }

    /**
     * @brief Establece una nueva escala.
     * @param newScale Nuevo vector de escala.
     */
    void
        setScale(const EU::Vector3& newScale) { scale = newScale; }

    // ------------------------------------------------------------------------
    // Utilidades
    // ------------------------------------------------------------------------

    /**
     * @brief Establece posición, rotación y escala simultáneamente.
     * * @param newPos Nueva posición.
     * @param newRot Nueva rotación.
     * @param newSca Nueva escala.
     */
    void
        setTransform(const EU::Vector3& newPos, const EU::Vector3& newRot, const EU::Vector3& newSca) {
        position = newPos;
        rotation = newRot;
        scale = newSca;
    }

public:
    /**
     * @brief Matriz de transformación resultante (World Matrix).
     * @details Accesible públicamente para ser consumida por el sistema de renderizado.
     */
    XMMATRIX matrix;

private:
    EU::Vector3 position;  ///< Coordenadas XYZ de posición.
    EU::Vector3 rotation;  ///< Rotación en ejes XYZ (Euler angles).
    EU::Vector3 scale;     ///< Factor de escala en ejes XYZ.
};