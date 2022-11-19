#include "helper.h"

namespace editor::gui {
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