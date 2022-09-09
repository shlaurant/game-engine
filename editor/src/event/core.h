#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

namespace editor {
    struct event {
        virtual ~event() = default;
    };

    class listener_instance {
    public:
        virtual void on_event(std::shared_ptr<event> e) = 0;
        virtual ~listener_instance() = default;
    };

    template<typename T>
    class listener : public listener_instance {
    public:
        void on_event(std::shared_ptr<event> e) override {
            auto p = std::static_pointer_cast<T>(e);
            on_event_t(p);
        }

        virtual void on_event_t(std::shared_ptr<T> e) = 0;

        virtual ~listener() = default;
    };

    class dispatcher {
        using event_id = size_t;

    public:
        template<typename T>
        void add_listener(std::shared_ptr<listener_instance> listener) {
            static_assert(std::is_base_of<event, T>::value);
            _listeners[get_id<T>()].push_back(listener);
        }

        template<typename T>
        void remove_listener(const std::shared_ptr<listener_instance> &listener) {
            static_assert(std::is_base_of<event, T>::value);
            auto &v = _listeners[get_id<T>()];
            auto it = find(v.begin(), v.end(), listener);
            if (it != v.end()) {
                v.erase(it);
            }
        }

        template<typename T, typename... Args>
        void post(Args &&...args) {
            auto p = std::make_shared<T>(std::forward<Args>(args)...);
            _events[get_id<T>()] = p;
        };

        void dispatch() {
            for (const auto &e: _events) {
                for (const auto &l: _listeners[e.first]) {
                    l->on_event(e.second);
                }
            }
            _events.clear();
        };

    private:
        std::unordered_map<event_id, std::vector<std::shared_ptr<listener_instance>>>
                _listeners;
        std::unordered_map<event_id, std::shared_ptr<event>> _events;

        template<typename T>
        event_id get_id() {
            static const event_id id = 0;
            return id;
        }
    };
}