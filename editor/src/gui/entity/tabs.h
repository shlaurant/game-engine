#pragma once

#include "entity_window.h"

namespace editor::gui::tab {
    const static int BuffSize = 24;
    const static float Align = 150.f;
    const static float ItemWidth = 80.f;

    bool information(entity_data &);
    bool transform(entity_data &);
    bool rigidbody(entity_data &);

    //helper method used as if it were macro
    bool input_text(const char *, char (&)[BuffSize], ImGuiInputTextFlags_);

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

    float to_float(char *arr);

#define INPUT_FLOAT(text, tag, out, T) static char tag[BuffSize] = ""; sprintf(tag, "%f", out); ImGui::Text(#text); \
ImGui::SameLine(); is_changed = is_changed || input_text<float>("##" #tag, tag, ImGuiInputTextFlags_CharsDecimal, out, to_float);
}