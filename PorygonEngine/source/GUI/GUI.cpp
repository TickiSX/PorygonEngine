#include "EngineUtilities\GUI\GUI.h"
#include "Viewport.h"
#include "Window.h"
#include "Device.h"
#include "DeviceContext.h"
#include "MeshComponent.h"
#include "ECS\Actor.h"
#include "EngineUtilities\Utilities\Camera.h"

// Variable estática para la operación actual de los Gizmos
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

void
GUI::init(Window& window, Device& device, DeviceContext& deviceContext) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Configuración de Flags
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    // Estilo visual "Apple Liquid" personalizado
    appleLiquidStyle(0.72f, ImVec4(0.0f, 0.515f, 1.0f, 1.0f));

    // Inicialización de Backends
    ImGui_ImplWin32_Init(window.m_hWnd);
    ImGui_ImplDX11_Init(device.m_device, deviceContext.m_deviceContext);

    toolTipData();
    selectedActorIndex = 0;
}

void
GUI::update(Viewport& viewport, Window& window) {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);

    // Dibujado de la interfaz principal de PorygonEngine
    drawStudioTopRibbon();
    drawEditorDockspace();
    closeApp();
    drawGizmoToolbar();
}

void
GUI::render() {
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void
GUI::destroy() {
    // PROTECCIÓN: Solo cerramos si el contexto y los datos del backend existen
    if (ImGui::GetCurrentContext() == nullptr) return;

    ImGuiIO& io = ImGui::GetIO();

    // Evita el crash "bd != nullptr" en imgui_impl_dx11.cpp
    if (io.BackendRendererUserData != nullptr) {
        ImGui_ImplDX11_Shutdown();
    }

    // Evita crashes en el backend de Win32
    if (io.BackendPlatformUserData != nullptr) {
        ImGui_ImplWin32_Shutdown();
    }

    ImGui::DestroyContext();
}

void
GUI::vec3Control(const std::string& label, float* values, float resetValue, float columnWidth) {
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

    // Corrección de API: Usar GetFontSize() público
    float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    // Eje X
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
    if (ImGui::Button("X", buttonSize)) values[0] = resetValue;
    ImGui::PopStyleColor(2);
    ImGui::SameLine();
    ImGui::DragFloat("##X", &values[0], 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    // Eje Y
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
    if (ImGui::Button("Y", buttonSize)) values[1] = resetValue;
    ImGui::PopStyleColor(2);
    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values[1], 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    // Eje Z
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
    if (ImGui::Button("Z", buttonSize)) values[2] = resetValue;
    ImGui::PopStyleColor(2);
    ImGui::SameLine();
    ImGui::DragFloat("##Z", &values[2], 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();
}

void GUI::toolTipData() {}

void
GUI::appleLiquidStyle(float opacity, ImVec4 accent) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    style.WindowRounding = 14.0f;
    style.ChildRounding = 14.0f;
    style.FrameRounding = 10.0f;
    style.PopupRounding = 14.0f;
    style.WindowPadding = ImVec2(14, 12);

    const float o = opacity;
    colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 0.95f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.18f, o);
    colors[ImGuiCol_Border] = ImVec4(1, 1, 1, 0.10f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.13f, 0.15f, o * 0.85f);
    colors[ImGuiCol_CheckMark] = accent;
    colors[ImGuiCol_SliderGrab] = accent;
    colors[ImGuiCol_Button] = ImVec4(0.13f, 0.13f, 0.15f, o);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.16f, 0.16f, 0.18f, o);
    colors[ImGuiCol_Header] = ImVec4(0.13f, 0.13f, 0.15f, o);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.22f, o);
    colors[ImGuiCol_DockingPreview] = ImVec4(accent.x, accent.y, accent.z, 0.35f);
}

void
GUI::closeApp() {
    if (show_exit_popup) {
        ImGui::OpenPopup("Exit?");
        show_exit_popup = false;
    }
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Exit?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Estas a punto de salir de la aplicacion.\nEstas seguro?\n\n");
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0))) { exit(0); }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}

void
GUI::inspectorGeneral(EU::TSharedPointer<Actor> actor) {
    if (!actor) return;
    ImGui::Begin("Inspector");

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
    ImGui::InputText("##ObjectName", &actor->getName()[0], 128);

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        inspectorContainer(actor);
    }
    ImGui::End();
}

void
GUI::inspectorContainer(EU::TSharedPointer<Actor> actor) {
    auto transform = actor->getComponent<Transform>();
    if (transform) {
        vec3Control("Position", (float*)transform->getPosition().data());
        vec3Control("Rotation", (float*)transform->getRotation().data());
        vec3Control("Scale", (float*)transform->getScale().data());
    }
}

void
GUI::outliner(const std::vector<EU::TSharedPointer<Actor>>& actors) {
    ImGui::Begin("Hierarchy");
    for (int i = 0; i < (int)actors.size(); ++i) {
        if (!actors[i]) continue;
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (selectedActorIndex == i) flags |= ImGuiTreeNodeFlags_Selected;

        bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", actors[i]->getName().c_str());
        if (ImGui::IsItemClicked()) selectedActorIndex = i;
        if (nodeOpen) ImGui::TreePop();
    }
    ImGui::End();
}

void GUI::editTransform(Camera& cam, Window& window, EU::TSharedPointer<Actor> actor)
{
    if (!actor) return;
    auto transform = actor->getComponent<Transform>();
    if (!transform) return;

    if (m_viewportSize.x < 64.0f || m_viewportSize.y < 64.0f) return;

    float* pos = (float*)transform->getPosition().data();
    float* rot = (float*)transform->getRotation().data();
    float* sca = (float*)transform->getScale().data();

    float mArr[16];
    ImGuizmo::RecomposeMatrixFromComponents(pos, rot, sca, mArr);

    float vArr[16], pArr[16];
    ToFloatArray(cam.getView(), vArr);
    ToFloatArray(cam.getProj(), pArr);

    ImGuizmo::SetDrawlist(m_viewportDrawList ? m_viewportDrawList : ImGui::GetForegroundDrawList());
    ImGuizmo::SetRect(m_viewportPos.x, m_viewportPos.y, m_viewportSize.x, m_viewportSize.y);

    ImGuizmo::Manipulate(vArr, pArr, mCurrentGizmoOperation, ImGuizmo::WORLD, mArr);

    if (ImGuizmo::IsUsing()) {
        float nP[3], nR[3], nS[3];
        ImGuizmo::DecomposeMatrixToComponents(mArr, nP, nR, nS);
        transform->setPosition(EU::Vector3(nP[0], nP[1], nP[2]));
        transform->setRotation(EU::Vector3(nR[0], nR[1], nR[2]));
        transform->setScale(EU::Vector3(nS[0], nS[1], nS[2]));
    }
}

void GUI::drawGizmoToolbar() {
    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin("GizmoToolBar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::Button("T")) mCurrentGizmoOperation = ImGuizmo::TRANSLATE; ImGui::SameLine();
        if (ImGui::Button("R")) mCurrentGizmoOperation = ImGuizmo::ROTATE; ImGui::SameLine();
        if (ImGui::Button("S")) mCurrentGizmoOperation = ImGuizmo::SCALE;
    }
    ImGui::End();
}

void GUI::drawStudioTopRibbon() {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x, vp->Pos.y + 24.0f));
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x, 72.0f));
    ImGui::Begin("##Ribbon", nullptr, ImGuiWindowFlags_NoDecoration);
    if (ImGui::Button("Move", ImVec2(60, 40))) mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::End();
}

void GUI::drawViewportPanel(ID3D11ShaderResourceView* viewportSRV) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");
    m_viewportDrawList = ImGui::GetWindowDrawList();
    m_viewportPos = ImGui::GetCursorScreenPos();
    m_viewportSize = ImGui::GetContentRegionAvail();
    if (viewportSRV) ImGui::Image((void*)viewportSRV, m_viewportSize);
    ImGui::End();
    ImGui::PopStyleVar();
}

void GUI::drawEditorDockspace() {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x, vp->Pos.y + 96.0f));
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x, vp->Size.y - 96.0f));
    ImGui::Begin("##DockContainer", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
    ImGui::DockSpace(ImGui::GetID("##EditorDS"), ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
}