#pragma once

#include "entity_window.h"

namespace editor::gui::tab {
    const static int BuffSize = 64;
    const static float Align = 150.f;
    const static float ItemWidth = 80.f;

    bool information(entity_data &);
    bool transform(entity_data &);
    bool rigidbody(entity_data &);
    bool collider(entity_data &);
    bool sprite(entity_data &);
    bool animation(entity_data &);
    bool text(entity_data &);

    //helper method used as if it were macro
    bool input_text(const char *, char (&)[BuffSize], ImGuiInputTextFlags_);

    template<typename T>
    void delete_context_menu(entity_data &entity) {
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::Button("delete")) {
                entity.delete_comp<T>();
            }
            ImGui::EndPopup();
        }
    }

    template<typename T>
    bool input_text(const char *tag, char (&buf)[BuffSize],
                    ImGuiInputTextFlags_ flags, T &out,
                    const std::function<T(char *)> &cf) {
        bool is_changed = false;
        if (input_text(tag, buf, flags)) {
            out = cf(buf);
            is_changed = true;
        }
        return is_changed;
    }

    float to_float(const char *arr);

    fuse::uuid64 to_uuid64(const char *arr);

    uint8_t to_uint8(const char *arr);

    std::string to_str(const char *arr);


#define PREFIX(T, name) if(!entity.has_comp<T>()) return false; \
bool is_changed = false; if(ImGui::TreeNode(name)) {             \
delete_context_menu<T>(entity); ImGui::PushItemWidth(ItemWidth); ImGui::Indent();

#define SUFFIX(T) ImGui::Unindent(); ImGui::PopItemWidth(); ImGui::TreePop(); \
} else {delete_context_menu<T>(entity);} return is_changed;

#define INPUT_FLOAT(text, tag, out) static char tag[BuffSize] = ""; sprintf(tag, "%f", out); ImGui::Text(#text); \
ImGui::SameLine(); is_changed = is_changed || input_text<float>("##" #tag, tag, ImGuiInputTextFlags_CharsDecimal, out, to_float);

#define INPUT_FLOAT_A(text, tag, out, align) static char tag[BuffSize] = ""; sprintf(tag, "%f", out); ImGui::Text(#text); \
ImGui::SameLine(align); is_changed = is_changed || input_text<float>("##" #tag, tag, ImGuiInputTextFlags_CharsDecimal, out, to_float);

#define  INPUT_INT(text, tag, out) static char tag[BuffSize] = ""; \
sprintf(tag, "%d", out); ImGui::Text(#text); ImGui::SameLine();   \
is_changed = is_changed || input_text<int>("##" #tag, tag, ImGuiInputTextFlags_CharsHexadecimal, out, atoi);

#define INPUT_UUID64(text, tag, out) static char tag[BuffSize] = ""; \
sprintf(tag, "%llu", out); ImGui::Text(#text); ImGui::SameLine();   \
is_changed = is_changed || input_text<fuse::uuid64>("##" #tag, tag, ImGuiInputTextFlags_CharsHexadecimal, out, to_uuid64);

#define  INPUT_UINT8(text, tag, out) static char tag[BuffSize] = ""; \
sprintf(tag, "%d", out); ImGui::Text(#text); ImGui::SameLine();   \
is_changed = is_changed || input_text<uint8_t>("##" #tag, tag, ImGuiInputTextFlags_CharsHexadecimal, out, to_uint8);

#define INPUT_STR(text, tag, out) static char tag[BuffSize] = ""; \
sprintf(tag, "%s", out.c_str()); ImGui::Text(#text); ImGui::SameLine();   \
is_changed = is_changed || input_text<std::string>("##" #tag, tag, ImGuiInputTextFlags_None, out, to_str);
}