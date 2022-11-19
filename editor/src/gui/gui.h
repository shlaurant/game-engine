#pragma once

#include <memory>
#include <map>
#include "event/core.h"
#include "core/typeid.h"

namespace editor::gui {
    class window {
    public:
        virtual void open() { _is_open = true; }
        virtual void close() { _is_open = false; }
        virtual void show() = 0;
        virtual ~window() = default;

    protected:
        bool _is_open = false;
    };

    class gui {
    public:
        static gui *instance() {
            static gui _instance;
            return &_instance;
        }

        template<typename T>
        void add_window(std::shared_ptr<T> w) {
            auto id = fuse::type_id<T>();
            _windows[id] = w;
        }

        template<typename T>
        std::shared_ptr<T> p_window() {
            return std::static_pointer_cast<T>(_windows[fuse::type_id<T>()]);
        }

        void show();

    private:
        gui();

        std::map<uint32_t, std::shared_ptr<window>> _windows;
    };
}

#define INPUT_FLOAT(text, tag, out) static char tag[BuffSize] = ""; sprintf(tag, "%f", out); ImGui::Text(#text); \
ImGui::SameLine(); input_text<float>("##" #tag, tag, ImGuiInputTextFlags_CharsDecimal, out, to_float);

#define INPUT_FLOAT_A(text, tag, out, align) static char tag[BuffSize] = ""; sprintf(tag, "%f", out); ImGui::Text(#text); \
ImGui::SameLine(align); input_text<float>("##" #tag, tag, ImGuiInputTextFlags_CharsDecimal, out, to_float);

#define  INPUT_INT(text, tag, out) static char tag[BuffSize] = ""; \
sprintf(tag, "%d", out); ImGui::Text(#text); ImGui::SameLine();   \
input_text<int>("##" #tag, tag, ImGuiInputTextFlags_CharsHexadecimal, out, atoi);

#define INPUT_UUID64(text, tag, out) static char tag[BuffSize] = ""; \
sprintf(tag, "%llu", out); ImGui::Text(#text); ImGui::SameLine();   \
input_text<fuse::uuid64>("##" #tag, tag, ImGuiInputTextFlags_CharsHexadecimal, out, to_uuid64);

#define  INPUT_UINT8(text, tag, out) static char tag[BuffSize] = ""; \
sprintf(tag, "%d", out); ImGui::Text(#text); ImGui::SameLine();   \
input_text<uint8_t>("##" #tag, tag, ImGuiInputTextFlags_CharsHexadecimal, out, to_uint8);

#define INPUT_STR(text, tag, out) static char tag[BuffSize] = ""; \
sprintf(tag, "%s", out.c_str()); ImGui::Text(#text); ImGui::SameLine();   \
input_text<std::string>("##" #tag, tag, ImGuiInputTextFlags_None, out, to_str);