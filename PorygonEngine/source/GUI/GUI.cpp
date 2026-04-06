#include "EngineUtilities\GUI\GUI.h"
#include "Viewport.h"
#include "Window.h"
#include "Device.h"
#include "DeviceContext.h"
#include "MeshComponent.h" 
#include "ECS\Actor.h"
#include "EngineUtilities\Utilities\Camera.h"
#include <string> 

// Variable estática para la operación actual de los Gizmos
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

void GUI::awake() {}

void GUI::init(Window& window, Device& device, DeviceContext& deviceContext) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    setupLightTheme(); // Aunque se llame LightTheme, ahora aplica el tema oscuro

    ImGui_ImplWin32_Init(window.m_hWnd);
    ImGui_ImplDX11_Init(device.m_device, deviceContext.m_deviceContext);

    toolTipData();
    m_selectedActorIndex = -1;
}

void GUI::update(Viewport& viewport, Window& window) {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);

    drawStudioTopRibbon();
    drawEditorDockspace();
    closeApp();
    drawGizmoToolbar();
}

void GUI::render() {
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void GUI::destroy() {
    if (ImGui::GetCurrentContext() == nullptr) return;
    ImGuiIO& io = ImGui::GetIO();
    if (io.BackendRendererUserData != nullptr) ImGui_ImplDX11_Shutdown();
    if (io.BackendPlatformUserData != nullptr) ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void GUI::setupLightTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Mantener las formas de pastilla
    style.WindowRounding = 12.0f;
    style.ChildRounding = 12.0f;
    style.FrameRounding = 20.0f;
    style.PopupRounding = 12.0f;
    style.TabRounding = 8.0f;
    style.GrabRounding = 20.0f;

    style.WindowPadding = ImVec2(12, 12);
    style.FramePadding = ImVec2(10, 4);
    style.ItemSpacing = ImVec2(8, 6);

    style.WindowBorderSize = 0.0f;
    style.FrameBorderSize = 1.0f;

    // Paleta de Colores - TEMA OSCURO
    ImVec4 col_fondo_oscuro = ImVec4(0.12f, 0.12f, 0.13f, 1.00f); // Gris oscuro de fondo
    ImVec4 col_frame_oscuro = ImVec4(0.18f, 0.18f, 0.19f, 1.00f); // Gris un poco más claro para inputs/botones
    ImVec4 col_cyan = ImVec4(0.00f, 0.70f, 0.95f, 1.00f);         // Tu Cyan
    ImVec4 col_cyan_hover = ImVec4(0.10f, 0.80f, 1.00f, 1.00f);   // Cyan brillante al pasar el ratón
    ImVec4 col_azul_oscuro = ImVec4(0.00f, 0.40f, 0.65f, 1.00f);  // Cyan oscuro para elementos activos
    ImVec4 col_texto_blanco = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    ImVec4 col_texto_desactivado = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    ImVec4 col_borde = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);        // Borde gris sutil

    colors[ImGuiCol_Text] = col_texto_blanco;
    colors[ImGuiCol_TextDisabled] = col_texto_desactivado;
    colors[ImGuiCol_WindowBg] = col_fondo_oscuro;
    colors[ImGuiCol_ChildBg] = col_fondo_oscuro;
    colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);

    // Bordes
    colors[ImGuiCol_Border] = col_borde;
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Fondos de inputs y botones (Gris medio)
    colors[ImGuiCol_FrameBg] = col_frame_oscuro;
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = col_azul_oscuro;

    // Headers (Desplegables)
    colors[ImGuiCol_Header] = col_frame_oscuro;
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
    colors[ImGuiCol_HeaderActive] = col_azul_oscuro;

    // Botones
    colors[ImGuiCol_Button] = col_frame_oscuro;
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
    colors[ImGuiCol_ButtonActive] = col_azul_oscuro;

    // Elementos de interacción
    colors[ImGuiCol_CheckMark] = col_cyan;
    colors[ImGuiCol_SliderGrab] = col_cyan;
    colors[ImGuiCol_SliderGrabActive] = col_cyan_hover;

    // Pestañas (Tabs)
    colors[ImGuiCol_Tab] = col_frame_oscuro;
    colors[ImGuiCol_TabHovered] = col_cyan_hover;
    colors[ImGuiCol_TabActive] = col_cyan;
    colors[ImGuiCol_TabUnfocused] = col_frame_oscuro;
    colors[ImGuiCol_TabUnfocusedActive] = col_frame_oscuro;

    colors[ImGuiCol_DockingPreview] = ImVec4(col_cyan.x, col_cyan.y, col_cyan.z, 0.60f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);

    colors[ImGuiCol_Separator] = col_borde;
    colors[ImGuiCol_SeparatorHovered] = col_cyan_hover;
    colors[ImGuiCol_SeparatorActive] = col_cyan;
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ResizeGripHovered] = col_cyan;
    colors[ImGuiCol_ResizeGripActive] = col_cyan_hover;

    colors[ImGuiCol_TitleBg] = col_fondo_oscuro;
    colors[ImGuiCol_TitleBgActive] = col_fondo_oscuro;
    colors[ImGuiCol_TitleBgCollapsed] = col_fondo_oscuro;
}

void GUI::drawStudioTopRibbon() {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    float ribbonHeight = 85.0f;

    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x, vp->Pos.y));
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x, ribbonHeight));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

    // Fondo de la barra superior (Gris muy oscuro para contrastar)
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.11f, 1.0f));
    ImGui::Begin("##Ribbon", nullptr, flags);
    ImGui::PopStyleColor();

    ImGui::SetCursorPos(ImVec2(10, 10));

    ImVec2 btnSize(75, 65);

    // Forzar botones muy redondeados
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 35.0f);

    auto DrawRibbonButton = [&](const char* topText, const char* bottomText, ImGuizmo::OPERATION op) {
        bool active = (mCurrentGizmoOperation == op);

        if (active) {
            // Cyan oscuro para el botón activo en el ribbon
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.00f, 0.40f, 0.65f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        }

        std::string label = std::string(topText) + "\n" + std::string(bottomText);
        if (ImGui::Button(label.c_str(), btnSize)) {
            mCurrentGizmoOperation = op;
        }

        if (active) ImGui::PopStyleColor(2);
        ImGui::SameLine();
        };

    DrawRibbonButton("Select", "Cursor", (ImGuizmo::OPERATION)0);
    DrawRibbonButton("Move", "W", ImGuizmo::TRANSLATE);
    DrawRibbonButton("Scale", "R", ImGuizmo::SCALE);
    DrawRibbonButton("Rotate", "E", ImGuizmo::ROTATE);
    DrawRibbonButton("Transform", "Tool", ImGuizmo::BOUNDS);

    ImGui::SameLine();
    ImGui::TextDisabled(" | ");
    ImGui::SameLine();

    ImGui::Button("Explorer\nPanel", btnSize); ImGui::SameLine();
    ImGui::Button("Properties\nPanel", btnSize); ImGui::SameLine();
    ImGui::Button("Toolbox\nAssets", btnSize);

    ImGui::PopStyleVar();
    ImGui::End();
}

void GUI::outliner(const std::vector<EU::TSharedPointer<Actor>>& actors) {
    ImGui::Begin("Hierarchy");

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::InputTextWithHint("##Search", "Search...", m_hierarchySearchBuffer, IM_ARRAYSIZE(m_hierarchySearchBuffer));
    ImGui::PopStyleVar();

    ImGui::Separator();

    ImGui::BeginChild("##ActorList");
    for (int i = 0; i < (int)actors.size(); ++i) {
        if (!actors[i]) continue;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        if (m_selectedActorIndex == i) flags |= ImGuiTreeNodeFlags_Selected;

        std::string displayName = "[-] " + actors[i]->getName();

        ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", displayName.c_str());

        if (ImGui::IsItemClicked()) m_selectedActorIndex = i;
    }
    ImGui::EndChild();
    ImGui::End();
}

void GUI::inspectorGeneral(EU::TSharedPointer<Actor> actor) {
    ImGui::Begin("Inspector");

    if (!actor) {
        ImGui::TextColored(ImGui::GetStyle().Colors[ImGuiCol_TextDisabled], "No actor selected.");
        ImGui::End();
        return;
    }

    ImGui::BeginChild("##InspectorHeader", ImVec2(0, 90), true);

    ImGui::Text("[-] "); ImGui::SameLine();

    static int lastSelected = -1;
    if (m_selectedActorIndex != lastSelected) {
        strcpy_s(m_inspectorNameBuffer, actor->getName().c_str());
        lastSelected = m_selectedActorIndex;
    }

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 20);
    if (ImGui::InputText("##ObjName", m_inspectorNameBuffer, IM_ARRAYSIZE(m_inspectorNameBuffer))) {
        actor->setName(m_inspectorNameBuffer);
    }

    ImGui::Dummy(ImVec2(0, 5));

    float halfWidth = ImGui::GetContentRegionAvail().x * 0.5f;

    ImGui::Text("Tag"); ImGui::SameLine();
    ImGui::SetNextItemWidth(halfWidth - 40);
    const char* tags[] = { "Untagged", "Player", "Enemy" };
    ImGui::Combo("##Tag", &m_selectedTagIndex, tags, IM_ARRAYSIZE(tags));

    ImGui::SameLine();

    ImGui::Text("Layer"); ImGui::SameLine();
    ImGui::SetNextItemWidth(halfWidth - 50);
    const char* layers[] = { "Default", "UI", "Water" };
    ImGui::Combo("##Layer", &m_selectedLayerIndex, layers, IM_ARRAYSIZE(layers));

    ImGui::EndChild();

    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        inspectorContainer(actor);
    }

    if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Mesh Filter"); ImGui::SameLine();
        ImGui::Button("Hacha_UV_OBJ", ImVec2(ImGui::GetContentRegionAvail().x, 0));
    }

    if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::DragFloat3("Light Direction", m_debugLightDir, 0.01f);
        ImGui::ColorEdit3("Light Color", m_debugLightColor);
    }

    ImGui::End();
}

void GUI::vec3Control(const std::string& label, float* values, float resetValue, float columnWidth) {
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 4, 0 });

    float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight, lineHeight };

    auto DrawVectorAxis = [&](const char* axisLabel, float* value, ImVec4 color, ImVec4 colorHover) {
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colorHover);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
        if (ImGui::Button(axisLabel, buttonSize)) *value = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine(0, 2);

        std::string dragLabel = "##" + std::string(axisLabel);
        ImGui::DragFloat(dragLabel.c_str(), value, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        };

    DrawVectorAxis("X", &values[0], ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f }, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::SameLine();
    DrawVectorAxis("Y", &values[1], ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f }, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
    ImGui::SameLine();
    DrawVectorAxis("Z", &values[2], ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f }, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });

    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();
}

void GUI::inspectorContainer(EU::TSharedPointer<Actor> actor) {
    auto transform = actor->getComponent<Transform>();
    if (transform) {
        vec3Control("Position", (float*)transform->getPosition().data());
        vec3Control("Rotation", (float*)transform->getRotation().data());
        vec3Control("Scale", (float*)transform->getScale().data(), 1.0f);
    }
}

void GUI::drawEditorDockspace() {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    float ribbonHeight = 85.0f; // Ajustado para igualar al Ribbon superior
    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x, vp->Pos.y + ribbonHeight));
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x, vp->Size.y - ribbonHeight));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("##DockContainer", nullptr, flags);
    ImGui::PopStyleVar();

    ImGuiID dockspace_id = ImGui::GetID("##EditorDS");
    ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();
}

void GUI::ToolBar() {}
void GUI::toolTipData() {}

void GUI::closeApp() {
    if (m_showExitPopup) {
        ImGui::OpenPopup("Exit?");
        m_showExitPopup = false;
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

void GUI::drawViewportPanel(ID3D11ShaderResourceView* viewportSRV) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");
    m_viewportDrawList = ImGui::GetWindowDrawList();
    m_viewportPos = ImGui::GetCursorScreenPos();
    m_viewportSize = ImGui::GetContentRegionAvail();

    m_viewportHovered = ImGui::IsWindowHovered();
    m_viewportFocused = ImGui::IsWindowFocused();

    if (viewportSRV) ImGui::Image((void*)viewportSRV, m_viewportSize);
    ImGui::End();
    ImGui::PopStyleVar();
}

void GUI::drawGizmoToolbar() {
    if (!m_viewportDrawList) return;

    ImGui::SetNextWindowPos(ImVec2(m_viewportPos.x + 10, m_viewportPos.y + 10));
    ImGui::SetNextWindowBgAlpha(0.5f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

    if (ImGui::Begin("##GizmoOverlay", nullptr, flags)) {
        float bSize = 25.0f;
        if (ImGui::Button("T", ImVec2(bSize, bSize))) mCurrentGizmoOperation = ImGuizmo::TRANSLATE; ImGui::SameLine();
        if (ImGui::Button("R", ImVec2(bSize, bSize))) mCurrentGizmoOperation = ImGuizmo::ROTATE; ImGui::SameLine();
        if (ImGui::Button("S", ImVec2(bSize, bSize))) mCurrentGizmoOperation = ImGuizmo::SCALE;
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

    ImGuizmo::SetDrawlist(m_viewportDrawList);
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