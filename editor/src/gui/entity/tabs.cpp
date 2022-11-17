#include <imgui.h>
#include "data/physics.h"
#include "data/common.h"
#include "data/graphics.h"
#include "data/behaviour.h"
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

        PREFIX(transform_data, "transform")

            auto p_tr = entity.comp_data<transform_data>();

            ImGui::Text("translation: ");
            ImGui::SameLine(Align);
            INPUT_FLOAT(x, tr_x, p_tr->translate.x);
            ImGui::SameLine();
            INPUT_FLOAT(y, tr_y, p_tr->translate.y);

            INPUT_FLOAT_A(rotation:, rot, p_tr->rotation, Align)

            ImGui::Text("scale: ");
            ImGui::SameLine(Align);
            INPUT_FLOAT(x, sc_x, p_tr->scale.x)
            ImGui::SameLine();
            INPUT_FLOAT(y, sc_y, p_tr->scale.y)

        SUFFIX(transform_data)


        return is_changed;
    }

    bool rigidbody(entity_data &entity) {
        PREFIX(rigidbody_data, "rigidbody")

            auto rb = entity.comp_data<rigidbody_data>();

            if (ImGui::Checkbox("disable", &(rb->disabled)) &&
                ImGui::IsItemDeactivatedAfterEdit()) {
                is_changed = true;
            }

            INPUT_FLOAT(gravity_scale, gs, rb->gravity_scale)

            ImGui::Text("velocity");
            ImGui::SameLine();
            INPUT_FLOAT(x, vx, rb->velocity.x)
            ImGui::SameLine();
            INPUT_FLOAT(y, vy, rb->velocity.y);

            ImGui::Text("force");
            ImGui::SameLine();
            INPUT_FLOAT(x, fx, rb->force.x);
            ImGui::SameLine();
            INPUT_FLOAT(y, fy, rb->force.y);

        SUFFIX(rigidbody_data)
    }

    bool collider(entity_data &entity) {
        PREFIX(collider_data, "collider")

            auto p_col = entity.comp_data<collider_data>();

            if (ImGui::Checkbox("disable", &(p_col->disabled)) &&
                ImGui::IsItemDeactivatedAfterEdit()) {
                is_changed = true;
            }

            ImGui::Text("size: ");
            ImGui::SameLine();
            INPUT_FLOAT(w, col_w, p_col->w);
            ImGui::SameLine();
            INPUT_FLOAT(h, col_h, p_col->h);

        SUFFIX(collider_data)
    }

    bool sprite(entity_data &entity) {
        PREFIX(sprite_data, "sprite")

            auto p = entity.comp_data<sprite_data>();
            INPUT_UUID64(sprite, sprite, p->sprite)
            INPUT_INT(flip, flip, p->flip);

        SUFFIX(sprite_data)
    }

    bool animation(entity_data &entity) {
        PREFIX(animation_data, "animation")

            auto p = entity.comp_data<animation_data>();
            INPUT_UUID64(animation, animation, p->animation);
            INPUT_INT(flip, flip, p->flip);

        SUFFIX(animation_data)
    }

    bool text(entity_data &entity) {
        PREFIX(text_data, "text")

            auto p = entity.comp_data<text_data>();
            INPUT_UUID64(font, font, p->font)
            INPUT_STR(text, text, p->text)
            INPUT_INT(flip, flip, p->flip)
            ImGui::Text("color");
            ImGui::SameLine(90.f);
            INPUT_UINT8(r, c_r, p->color.r);
            ImGui::SameLine();
            INPUT_UINT8(g, c_g, p->color.g);
            ImGui::SameLine();
            INPUT_UINT8(b, c_b, p->color.b);
            ImGui::SameLine();
            INPUT_UINT8(a, c_a, p->color.a);

        SUFFIX(text_data)
    }

    bool script(entity_data &entity) {
        PREFIX(script_data, "script")

        auto p = entity.comp_data<script_data>();
        INPUT_STR(script, script_name, p->name);

        SUFFIX(script_data)
    }

    bool input_text(const char *tag, char (&buf)[BuffSize],
                    ImGuiInputTextFlags_ flags = ImGuiInputTextFlags_None) {
        return ImGui::InputText(tag, buf, BuffSize, flags) &&
               ImGui::IsItemDeactivatedAfterEdit();
    }

    float to_float(const char *arr) {
        return (float) atof(arr);
    }

    fuse::uuid64 to_uuid64(const char *arr) {
        char *end;
        return strtoull(arr, &end, 0);
    }

    std::string to_str(const char *arr) {
        return {arr};
    }

    uint8_t to_uint8(const char *arr) {
        return (uint8_t) atoi(arr);
    }
}
