#pragma once
#include "Prerequisites.h"

class Device {
public:
  Device() = default;
  ~Device() = default;

  // Si ya tienes creado el ID3D11Device en otro lado, p?salo aqu?:
  void init(ID3D11Device* existing) { m_device = existing; }
  void update() {}
  void render() {}
  void destroy() { /* si t? no creas el device aqu?, NO lo liberes aqu? */ }

  HRESULT CreateRenderTargetView(ID3D11Resource* pResource,
    const D3D11_RENDER_TARGET_VIEW_DESC* pDesc,
    ID3D11RenderTargetView** ppRTView);

  // Helpers opcionales
  ID3D11Device* Get() const { return m_device; }
  void Set(ID3D11Device* dev) { m_device = dev; }

public:
  /**
   * @brief Puntero al dispositivo Direct3D 11.
   * Creado fuera y asignado con init(Set), usado aqu?.
   */
  ID3D11Device* m_device = nullptr;
};