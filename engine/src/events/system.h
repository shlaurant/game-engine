#pragma once

#include "core/event.h"

namespace fuse {
    struct quite_event : event_instance {
        FUSE_INLINE quite_event() = default;
    };

    struct keydown_event : event_instance {
        FUSE_INLINE keydown_event(int key) : key(key) {}
        int key;
    };

    struct keyup_event : event_instance {
        FUSE_INLINE keyup_event(int key) : key(key) {}
        int key;
    };

    struct mousedown_event : event_instance {
        FUSE_INLINE mousedown_event(int btn) : button(btn) {}
        int button;
    };

    struct mouseup_event : event_instance {
        FUSE_INLINE mouseup_event(int btn) : button(btn) {}
        int button;
    };

    struct mousemotion_event : event_instance {
        FUSE_INLINE mousemotion_event() = default;
    };

    struct mousewheel_event : event_instance {
        FUSE_INLINE mousewheel_event() = default;
    };
}