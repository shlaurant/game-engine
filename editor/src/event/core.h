#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

namespace editor {
    struct event {
        virtual ~event() = default;
    };

    class listener {
    public:
        virtual void on_event(std::shared_ptr<event> e) = 0;
        virtual ~listener() = default;
    };

    class dispatcher {
        using event_id = size_t;

    public:
        template<typename T>
        void add_listener(std::shared_ptr<listener> listener) {
            static_assert(std::is_base_of<event, T>::value);
            _listeners[get_id<T>()].push_back(listener);
        }

        template<typename T>
        void remove_listener(std::shared_ptr<listener> listener) {
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
        std::unordered_map<event_id, std::vector<std::shared_ptr<listener>>>
                _listeners;
        std::unordered_map<event_id, std::shared_ptr<event>> _events;

        template<typename T>
        event_id get_id() {
            static const event_id id = 0;
            return id;
        }
    };
}