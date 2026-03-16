#pragma once
#include "Prerequisites.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Buffer.h"
#include "SamplerState.h"
#include "Model3D.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "EngineUtilities\Utilities\Camera.h"
#include "ECS\Actor.h"

class Device;
class DeviceContext;

/**
 * @class Skybox
 * @brief Gestiona la representación del fondo infinito de la escena.
 *
 * Esta clase encapsula todos los estados de renderizado necesarios para dibujar
 * un cubo envolvente con una textura de cubemap, incluyendo el shader, estados
 * de rasterización y estados de profundidad/esténcil específicos.
 */
class
	Skybox {
public:
	/**
	 * @brief Constructor por defecto.
	 */
	Skybox() = default;

	/**
	 * @brief Destructor por defecto.
	 */
	~Skybox() = default;

	/**
	 * @brief Inicializa los recursos del Skybox.
	 *
	 * Configura el modelo del cubo, carga los shaders y vincula la textura de cubemap.
	 *
	 * @param device        Dispositivo para la creación de recursos.
	 * @param deviceContext Contexto para la carga inicial de datos.
	 * @param cubemap       Referencia a la textura de tipo cubemap a utilizar.
	 * @return @c S_OK si la inicialización fue exitosa.
	 */
	HRESULT
		init(Device& device, DeviceContext* deviceContext, Texture& cubemap);

	/**
	 * @brief Actualiza la lógica del Skybox.
	 *
	 * Ajusta la posición del cubo para que siempre siga a la cámara, creando la
	 * ilusión de distancia infinita.
	 *
	 * @param deviceContext Contexto del dispositivo para actualizar buffers.
	 * @param camera        Cámara activa para obtener las matrices de vista y posición.
	 */
	void
		update(DeviceContext& deviceContext, Camera& camera);

	/**
	 * @brief Realiza el renderizado del Skybox.
	 *
	 * Configura los estados de rasterización (Cull None) y profundidad (Less Equal)
	 * antes de emitir la orden de dibujo.
	 *
	 * @param deviceContext Contexto del dispositivo gráfico.
	 */
	void
		render(DeviceContext& deviceContext);

	/**
	 * @brief Libera los recursos asociados al Skybox.
	 */
	void
		destroy() {}

private:
	ShaderProgram m_shaderProgram;      ///< Programa de shaders (VS/PS) para el Skybox.
	Buffer m_constantBuffer;           ///< Buffer de constantes para matrices.
	SamplerState m_samplerState;       ///< Estado de muestreo para la textura.
	RasterizerState m_rasterizerState; ///< Estado de rasterización (Culling desactivado).
	DepthStencilState m_depthStencilState; ///< Estado de profundidad para el fondo.
	Texture m_skyboxTexture;           ///< Referencia a la textura de entorno.
	Model3D* m_cubeModel = nullptr;    ///< Puntero al modelo geométrico del cubo.

	/**
	 * @brief Actor que representa la entidad del Skybox en el grafo de escena.
	 */
	EU::TSharedPointer<Actor> m_skybox;
};