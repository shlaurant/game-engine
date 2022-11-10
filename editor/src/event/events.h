#pragma once

#include "core.h"
#include "data/entity_data.h"

namespace editor {
    struct entity_sel_event : event {
        entity_sel_event(entity_data data) : data(data) {}

        entity_data data;
    };

    struct entity_change_event : event {
        entity_change_event(entity_data data) : new_data(data) {}

        entity_data new_data;
    };
}