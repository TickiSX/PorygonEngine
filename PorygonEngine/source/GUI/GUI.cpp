#include "EngineUtilities\GUI\GUI.h"
#include "Window.h"
#include "Device.h"
#include "DeviceContext.h"
#include "MeshComponent.h"
#include "ECS\Actor.h"
#include "ImGuizmo.h"

// Variable estática para la operación actual del Gizmo
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

void
GUI::init(Window& window, Device& device, DeviceContext& deviceContext) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    appleLiquidStyle(0.72f, ImVec4(0.0f, 0.515f, 1.0f, 1.0f));

    ImGui_ImplWin32_Init(window.m_hWnd);
    ImGui_ImplDX11_Init(device.m_device, deviceContext.m_deviceContext);

    toolTipData();
    selectedActorIndex = 0;
}

void
GUI::update(Window& window) {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetRect(0, 0, (float)window.m_width, (float)window.m_height);

    ToolBar();
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
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void
GUI::vec3Control(const std::string& label, float* values, float resetValue, float columnWidth) {
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

    float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    // X
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    if (ImGui::Button("X", buttonSize)) values[0] = resetValue;
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::DragFloat("##X", &values[0], 0.1f);
    ImGui::PopItemWidth();
    ImGui::SameLine();

    // Y
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    if (ImGui::Button("Y", buttonSize)) values[1] = resetValue;
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values[1], 0.1f);
    ImGui::PopItemWidth();
    ImGui::SameLine();

    // Z
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    if (ImGui::Button("Z", buttonSize)) values[2] = resetValue;
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::DragFloat("##Z", &values[2], 0.1f);
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
    style.FrameRounding = 10.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowPadding = ImVec2(14, 12);

    const float o = opacity;
    colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 0.95f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.18f, o);
    colors[ImGuiCol_Border] = ImVec4(1, 1, 1, 0.10f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.13f, 0.15f, o);
    colors[ImGuiCol_CheckMark] = accent;
    colors[ImGuiCol_SliderGrab] = accent;
    colors[ImGuiCol_Button] = ImVec4(1, 1, 1, 0.05f);
}

void
GUI::ToolBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) { show_exit_popup = true; }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
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
        if (ImGui::Button("OK", ImVec2(120, 0))) { exit(0); }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}

void
GUI::inspectorGeneral(EU::TSharedPointer<Actor> actor) {
    if (actor.isNull()) return;

    ImGui::Begin("Inspector");

    bool isStatic = false;
    ImGui::Checkbox("##Static", &isStatic);
    ImGui::SameLine();

    float availWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetNextItemWidth(availWidth * 0.6f);

    std::string name = actor->getName();
    char buf[128];
    strcpy_s(buf, name.c_str());
    if (ImGui::InputText("##ObjectName", buf, 128)) {
        actor->setName(buf);
    }

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
        // Obtenemos los datos del transform (X, Y, Z)
        vec3Control("Position", (float*)&transform->getPosition(), 0.0f);
        vec3Control("Rotation", (float*)&transform->getRotation(), 0.0f);
        vec3Control("Scale", (float*)&transform->getScale(), 1.0f);
    }
}

void
GUI::outliner(const std::vector<EU::TSharedPointer<Actor>>& actors) {
    ImGui::Begin("Hierarchy");

    static ImGuiTextFilter filter;
    filter.Draw("Search...", 180.0f);
    ImGui::Separator();

    for (int i = 0; i < (int)actors.size(); ++i) {
        if (actors[i].isNull()) continue;

        std::string name = actors[i]->getName();
        if (!filter.PassFilter(name.c_str())) continue;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Leaf;
        if (selectedActorIndex == i) flags |= ImGuiTreeNodeFlags_Selected;

        if (ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", name.c_str())) {
            if (ImGui::IsItemClicked()) selectedActorIndex = i;
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

void
GUI::editTransform(const XMMATRIX& view, const XMMATRIX& projection, EU::TSharedPointer<Actor> actor)
{
    if (actor.isNull()) return;
    auto transform = actor->getComponent<Transform>();
    if (!transform) return;

    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

    // 1. Obtener datos actuales del transform
    EU::Vector3 pos = transform->getPosition();
    EU::Vector3 rot = transform->getRotation();
    EU::Vector3 sca = transform->getScale();

    // 2. Crear una matriz temporal compatible con ImGuizmo a partir de los vectores
    float mArr[16];
    ImGuizmo::RecomposeMatrixFromComponents(&pos.x, &rot.x, &sca.x, mArr);

    // 3. Preparar matrices de Cámara (View y Projection)
    float vArr[16], pArr[16];
    XMStoreFloat4x4((XMFLOAT4X4*)vArr, view);
    XMStoreFloat4x4((XMFLOAT4X4*)pArr, projection);

    // 4. Manipular el Gizmo
    if (ImGuizmo::Manipulate(vArr, pArr, mCurrentGizmoOperation, mCurrentGizmoMode, mArr)) {
        // 5. Descomponer la matriz resultante de vuelta a vectores
        float newPos[3], newRot[3], newSca[3];
        ImGuizmo::DecomposeMatrixToComponents(mArr, newPos, newRot, newSca);

        // 6. Actualizar el transform del Actor
        transform->setPosition(EU::Vector3(newPos[0], newPos[1], newPos[2]));
        transform->setRotation(EU::Vector3(newRot[0], newRot[1], newRot[2]));
        transform->setScale(EU::Vector3(newSca[0], newSca[1], newSca[2]));
    }
}

void GUI::drawGizmoToolbar() {
    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.35f);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;

    if (ImGui::Begin("GizmoToolBar", nullptr, window_flags)) {
        auto buttonMode = [&](const char* label, ImGuizmo::OPERATION op, ImGuiKey key) {
            bool isActive = (mCurrentGizmoOperation == op);
            if (isActive) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));

            if (ImGui::Button(label) || ImGui::IsKeyPressed(key)) {
                mCurrentGizmoOperation = op;
            }

            if (isActive) ImGui::PopStyleColor();
            ImGui::SameLine();
            };

        buttonMode("T", ImGuizmo::TRANSLATE, ImGuiKey_W);
        buttonMode("R", ImGuizmo::ROTATE, ImGuiKey_E);
        buttonMode("S", ImGuizmo::SCALE, ImGuiKey_R);
    }
    ImGui::End();
}