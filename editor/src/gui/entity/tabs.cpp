#include <imgui.h>
#include <data/transform_data.h>
#include "data/info_data.h"
#include "tabs.h"

namespace editor::gui::tab {
    bool information(entity_data &entity) {
        bool is_changed = false;

        static char name[BuffSize] = "";
        static char tag[BuffSize] = "";

        strcpy_s(name, entity.comp_data<info_data>()->name.c_str());
        strcpy_s(tag, entity.comp_data<info_data>()->tag.c_str());

        if (ImGui::TreeNode("information")) {
            ImGui::Text("uuid: %llu", entity.comp_data<info_data>()->uuid);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("name: ");
            ImGui::SameLine();
            if (ImGui::InputText("##name", name, BuffSize) &&
                ImGui::IsItemDeactivatedAfterEdit()) {
                is_changed = true;
                entity.comp_data<info_data>()->name = std::string(name);
            }

            ImGui::AlignTextToFramePadding();
            ImGui::Text("tag: ");
            ImGui::SameLine();
            ImGui::InputText("##tag", tag, BuffSize);
            ImGui::TreePop();
        }

        return is_changed;
    }

    bool transform(entity_data &entity) {
        if (!entity.has_comp<transform_data>()) return false;
        if (ImGui::TreeNode("transform")) {
            ImGui::Indent();
            const static float align = 150.f;

            auto p_tr = entity.comp_data<transform_data>();

            ImGui::PushItemWidth(80.f);
            static char tr_x[BuffSize] = "";
            static char tr_y[BuffSize] = "";
            sprintf(tr_x, "%f", p_tr->translate.x);
            sprintf(tr_y, "%f", p_tr->translate.y);
            ImGui::Text("translation: ");
            ImGui::SameLine(align);
            ImGui::InputText("x", tr_x, BuffSize, ImGuiInputTextFlags_CharsDecimal);
            ImGui::SameLine();
            ImGui::InputText("y", tr_y, BuffSize, ImGuiInputTextFlags_CharsDecimal);

            static char rot[BuffSize] = "";
            sprintf(rot, "%f", p_tr->rotation);
            ImGui::Text("rotation: ");
            ImGui::SameLine(align);
            ImGui::InputText("##rotation", rot, BuffSize,
                             ImGuiInputTextFlags_CharsDecimal);

            static char sc_x[BuffSize] = "";
            static char sc_y[BuffSize] = "";
            sprintf(sc_x, "%f", p_tr->scale.x);
            sprintf(sc_y, "%f", p_tr->scale.y);
            ImGui::Text("scale: ");
            ImGui::SameLine(align);
            ImGui::InputText("x", sc_x, BuffSize, ImGuiInputTextFlags_CharsDecimal);
            ImGui::SameLine();
            ImGui::InputText("y", sc_y, BuffSize, ImGuiInputTextFlags_CharsDecimal);
            ImGui::PopItemWidth();

            ImGui::Unindent();
            ImGui::TreePop();
        }

        return false;
    }
}
