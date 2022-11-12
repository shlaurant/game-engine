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