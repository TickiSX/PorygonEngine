#pragma once
/**
 * @file Device.h
 * @brief Wrapper m�nimo sobre `ID3D11Device` para utilidades puntuales.
 *
 * @details
 * - Centraliza la propiedad del puntero `ID3D11Device*` y operaciones b�sicas.
 * - Expone un *thin wrapper* a `CreateRenderTargetView` con validaciones.
 * - El puntero `m_device` se deja **p�blico** por compatibilidad con tu c�digo actual.
 *
 * @note Las dependencias de Win32/D3D11/DirectXMath se incluyen v�a `Prerequisites.h`.
 */

#include "Prerequisites.h"

 /**
  * @class Device
  * @brief Encapsula un `ID3D11Device*` y ofrece utilidades m�nimas.
  *
  * @details
  * Esta clase NO crea el dispositivo por s� misma; espera que un tercero (p.ej.
  * `D3D11CreateDeviceAndSwapChain`) inicialice `m_device`. Provee:
  * - `destroy()` para liberar el dispositivo de forma segura.
  * - `CreateRenderTargetView(...)` como envoltura validada.
  *
  * @warning No es copiable para evitar doble liberaci�n del `ID3D11Device*`.
  * Si necesitas transferencia de propiedad, considera a�adir sem�ntica de movimiento.
  */
class Device {
public:
    /** @brief Ctor por defecto: no adquiere recursos. */
    Device() = default;

    /** @brief Dtor por defecto: no libera autom�ticamente (usa `destroy()`). */
    ~Device() = default;

    // --------------------------
    // Sem�ntica de copia/movimiento
    // --------------------------

    /**
     * @brief Copia deshabilitada para evitar doble `Release()`.
     */
    Device(const Device&) = delete;

    /**
     * @brief Asignaci�n por copia deshabilitada para evitar doble `Release()`.
     */
    Device& operator=(const Device&) = delete;

    // --------------------------
    // Gesti�n del recurso
    // --------------------------

    /**
     * @brief Libera el dispositivo si est� presente y lo pone a `nullptr`.
     *
     * @details
     * Internamente invoca `SAFE_RELEASE(m_device)`.
     * Debes llamar a este m�todo durante el shutdown de tu app/sistema gr�fico,
     * despu�s de limpiar todos los objetos creados por el device.
     *
     * @code
     * // ejemplo de uso
     * g_device.destroy();
     * @endcode
     */
    void destroy();

    // --------------------------
    // Envolturas (thin wrappers)
    // --------------------------

    /**
     * @brief Envoltura con validaciones a `ID3D11Device::CreateRenderTargetView`.
     *
     * @param pResource   Recurso de origen (t�picamente el backbuffer o una textura).
     * @param pDesc       Descriptor (puede ser `nullptr` para vista por defecto).
     * @param ppRTView    Salida: puntero a la vista creada (no nulo).
     * @return `S_OK` si tuvo �xito; un `HRESULT` de error en caso contrario.
     *
     * @pre `m_device != nullptr`
     * @pre `pResource != nullptr`
     * @pre `ppRTView != nullptr`
     *
     * @post En �xito, `*ppRTView` contendr� una referencia v�lida que deber�s liberar
     *       con `Release()` cuando ya no la uses.
     */
    HRESULT CreateRenderTargetView(ID3D11Resource* pResource,
        const D3D11_RENDER_TARGET_VIEW_DESC* pDesc,
        ID3D11RenderTargetView** ppRTView);

public:
    /**
     * @brief Puntero al dispositivo D3D11 subyacente.
     *
     * @details
     * Se expone **p�blico** por compatibilidad con c�digo existente (p.ej. creaci�n
     * de buffers/shaders directamente). Si quieres aislar mejor responsabilidades,
     * puedes hacerlo privado y a�adir m�todos proxy m�s adelante.
     */
    ID3D11Device* m_device = nullptr;
};