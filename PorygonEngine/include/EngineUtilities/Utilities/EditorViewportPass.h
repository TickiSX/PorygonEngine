#pragma once
#include "Prerequisites.h"
#include "Texture.h"
#include "RenderTargetView.h"
#include "DepthStencilView.h"

class Device;
class DeviceContext;

/**
 * @class EditorViewportPass
 * @brief Gestiona un pase de renderizado específico para el viewport del editor.
 *
 * Esta clase encapsula la creación y gestión de un Render Target y un Depth Stencil Buffer
 * personalizados, permitiendo renderizar la escena en una textura que puede ser
 * visualizada dentro de una interfaz de usuario (como ImGui).
 */
class
    EditorViewportPass {
public:
    /**
     * @brief Constructor por defecto.
     */
    EditorViewportPass() = default;

    /**
     * @brief Destructor por defecto.
     */
    ~EditorViewportPass() = default;

    /**
     * @brief Inicializa los recursos del pase de renderizado.
     *
     * @param device Dispositivo con el que se crearán las texturas y vistas.
     * @param width  Ancho inicial del viewport.
     * @param height Alto inicial del viewport.
     * @return @c S_OK si la creación fue exitosa; código @c HRESULT en caso contrario.
     */
    HRESULT
        init(Device& device, unsigned int width, unsigned int height);

    /**
     * @brief Redimensiona los buffers del pase de renderizado.
     *
     * @param device Dispositivo para recrear los recursos.
     * @param width  Nuevo ancho del viewport.
     * @param height Nuevo alto del viewport.
     * @return @c S_OK si la operación fue exitosa.
     */
    HRESULT
        resize(Device& device, unsigned int width, unsigned int height);

    /**
     * @brief Prepara el contexto para comenzar el renderizado en este pase.
     *
     * Establece el Render Target y limpia los buffers de color y profundidad.
     *
     * @param deviceContext Contexto del dispositivo donde se vincularán las vistas.
     * @param clearColor    Color de limpieza (RGBA).
     */
    void
        begin(DeviceContext& deviceContext, const float clearColor[4]);

    /**
     * @brief Intercambia los recursos entre dos pases de renderizado.
     * @param other Otra instancia de @c EditorViewportPass.
     */
    void
        swap(EditorViewportPass& other);

    /**
     * @brief Limpia únicamente el buffer de profundidad.
     * @param deviceContext Contexto del dispositivo.
     */
    void
        clearDepth(DeviceContext& deviceContext);

    /**
     * @brief Configura el Viewport en el pipeline de Direct3D.
     * @param deviceContext Contexto del dispositivo.
     */
    void
        setViewport(DeviceContext& deviceContext);

    /**
     * @brief Libera todos los recursos internos asociados al pase.
     */
    void
        destroy();

    /**
     * @brief Obtiene la vista de recurso de shader del buffer de color.
     * @return Puntero al @c ID3D11ShaderResourceView para uso en materiales o UI.
     */
    ID3D11ShaderResourceView* getSRV() const { return m_colorSRV.m_textureFromImg; }

    /**
     * @brief Obtiene el ancho actual del pase.
     * @return Valor entero del ancho.
     */
    unsigned int
        getWidth() const { return m_width; }

    /**
     * @brief Obtiene el alto actual del pase.
     * @return Valor entero del alto.
     */
    unsigned int
        getHeight() const { return m_height; }

    /**
     * @brief Verifica si todos los recursos internos están correctamente inicializados.
     * @return @c true si las texturas y vistas son válidas; @c false en caso contrario.
     */
    bool
        isValid() const {
        return m_colorTexture.m_texture != nullptr &&
            m_colorSRV.m_textureFromImg != nullptr &&
            m_depthTexture.m_texture != nullptr;
    }

private:
    /**
     * @brief Helper interno para la creación de texturas y vistas.
     */
    HRESULT
        createResources(Device& device, unsigned int width, unsigned int height);

private:
    Texture          m_colorTexture;  ///< Textura de color para el Render Target.
    Texture          m_colorSRV;      ///< Contenedor para el Shader Resource View de color.
    RenderTargetView m_rtv;           ///< Vista de destino de renderizado.

    Texture          m_depthTexture;  ///< Textura para el buffer de profundidad.
    DepthStencilView m_dsv;           ///< Vista de profundidad/esténcil.

    unsigned int     m_width = 1;     ///< Ancho actual del buffer.
    unsigned int     m_height = 1;    ///< Alto actual del buffer.
};