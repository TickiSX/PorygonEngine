#pragma once
#include "Prerequisites.h"
#include "EngineUtilities\Vectors\Vector3.h"

/**
 * @class Camera
 * @brief Gestiona la lógica de una cámara en el espacio 3D.
 *
 * Esta clase administra las matrices de Vista (View) y Proyección (Projection),
 * permitiendo movimientos relativos como Walk, Strafe y rotaciones Yaw/Pitch.
 * Utiliza un sistema de coordenadas Handedness (LH).
 */
class
    Camera {
public:
    /**
     * @brief Constructor por defecto.
     * Inicializa las matrices y los vectores base (Right, Up, Forward).
     */
    Camera();

    /**
     * @brief Destructor virtual por defecto.
     */
    ~Camera() = default;

    /**
     * @brief Configura la proyección en perspectiva (LH).
     *
     * @param fovYRadians  Campo de visión vertical en radianes.
     * @param aspectRatio Relación de aspecto (Ancho/Alto).
     * @param nearPlane   Distancia al plano cercano.
     * @param farPlane    Distancia al plano lejano.
     *
     * @note Calcula la matriz de proyección con @c XMMatrixPerspectiveFovLH.
     */
    void
        setLens(float fovYRadians, float aspectRatio, float nearPlane, float farPlane);

    /**
     * @brief Define posición en mundo a partir de componentes escalares.
     * @param x Coordenada X.
     * @param y Coordenada Y.
     * @param z Coordenada Z.
     */
    void
        setPosition(float x, float y, float z);

    /**
     * @brief Define posición en mundo.
     * @param pos Vector de posición.
     */
    void
        setPosition(const EU::Vector3& pos);

    /**
     * @brief Obtiene la posición en mundo (Copia).
     * @return Copia del @c EU::Vector3 de posición.
     */
    EU::Vector3
        getPosition() const { return m_position; }

    /**
     * @brief Obtiene la posición en mundo (Referencia).
     * @return Referencia al @c EU::Vector3 de posición.
     */
    EU::Vector3&
        getPosition() { return m_position; }

    /**
     * @brief Fuerza la cámara a mirar a un objetivo (LH).
     *
     * @param pos    Posición de la cámara.
     * @param target Punto al que debe mirar.
     * @param up     Vector de referencia superior (Por defecto 0,1,0).
     *
     * @note Calcula la base ortonormal y marca la vista como "dirty".
     */
    void
        lookAt(const EU::Vector3& pos,
            const EU::Vector3& target,
            const EU::Vector3& up = EU::Vector3(0, 1, 0));

    /**
     * @brief Movimiento relativo a la cámara (adelante/atrás).
     * @param d Distancia a desplazar.
     */
    void
        walk(float d);

    /**
     * @brief Movimiento relativo a la cámara (izquierda/derecha).
     * @param d Distancia a desplazar.
     */
    void
        strafe(float d);

    /**
     * @brief Rotación sobre el eje Y global (yaw).
     * @param radians Ángulo de rotación en radianes.
     */
    void
        yaw(float radians);

    /**
     * @brief Rotación sobre el eje Right local (pitch).
     * @param radians Ángulo de rotación en radianes.
     */
    void
        pitch(float radians);

    /**
     * @brief Recalcula la matriz View si es necesario.
     *
     * Reconstruye la base ortonormal (Right/Up/Forward) y calcula la matriz
     * final con @c XMMatrixLookToLH.
     */
    void
        updateViewMatrix();

    /**
     * @brief Obtiene la Matriz View (mundo->vista).
     * @return Matriz @c XMMATRIX de vista.
     */
    XMMATRIX
        getView() const { return XMLoadFloat4x4(&m_view); }

    /**
     * @brief Obtiene la Matriz Projection (vista->clip).
     * @return Matriz @c XMMATRIX de proyección.
     */
    XMMATRIX
        getProj() const { return XMLoadFloat4x4(&m_proj); }

    /**
     * @brief View sin traslación (solo rotación). Ideal para Skybox.
     * @return Matriz @c XMMATRIX con la fila de traslación en cero.
     */
    XMMATRIX
        GetViewNoTranslation() const {
        XMMATRIX v = getView();
        v.r[3] = XMVectorSet(0, 0, 0, 1);
        return v;
    }

    /**
     * @brief Devuelve parámetros de proyección.
     */
    float getFovY()   const { return m_fovY; }
    float getAspect() const { return m_aspectRatio; }
    float getNearZ()  const { return m_nearPlane; }
    float getFarZ()   const { return m_farPlane; }

    /**
     * @brief Obtiene los vectores base en espacio de mundo.
     */
    EU::Vector3 GetRight()   const { return m_right; }
    EU::Vector3 GetUp()      const { return m_up; }
    EU::Vector3 GetForward() const { return m_forward; }

    /**
     * @brief Utilidad para convertir de DirectXMath a Vector3 del motor.
     * @param v Vector de DirectXMath.
     * @return Instancia de @c EU::Vector3.
     */
    inline EU::Vector3
        FromXM(FXMVECTOR v) {
        XMFLOAT3 t;
        XMStoreFloat3(&t, v);
        return EU::Vector3(t.x, t.y, t.z);
    }

private:
    EU::Vector3 m_position; ///< Posición de la cámara en el espacio de mundo.

    EU::Vector3 m_right{ 1.0f, 0.0f, 0.0f };   ///< Vector Right de la base ortonormal.
    EU::Vector3 m_up{ 0.0f, 1.0f, 0.0f };      ///< Vector Up de la base ortonormal.
    EU::Vector3 m_forward{ 0.0f, 0.0f, 1.0f }; ///< Vector Forward de la base ortonormal.

    XMFLOAT4X4 m_view{}; ///< Matriz de vista almacenada.
    XMFLOAT4X4 m_proj{}; ///< Matriz de proyección almacenada.

    float m_fovY{ XM_PIDIV4 };  ///< Campo de visión vertical en radianes.
    float m_aspectRatio = 1.0f; ///< Relación de aspecto (ancho/alto).
    float m_nearPlane = 0.01f;  ///< Distancia al plano de recorte cercano.
    float m_farPlane = 1000.0f; ///< Distancia al plano de recorte lejano.

    bool m_viewDirty = true;    ///< Flag que indica si la matriz view requiere recálculo.
};