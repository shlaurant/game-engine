#include <imgui.h>
#include "asset_tabs.h"
#include "data/scene_data.h"
#include "gui/helper.h"

namespace editor::gui::asset_tab {
    void animation() {
        PREFIX_AS(animation)
        SUFFIX_AS
    }

    void audio() {
        PREFIX_AS(audio)
                    INPUT_STR(src, audio_src, p->source)
        SUFFIX_AS
    }

    void font() {
        PREFIX_AS(font)
                    INPUT_INT(size, font_sz, p->size)
                    INPUT_STR(src, texture_src, p->source)
        SUFFIX_AS
    }

    void texture() {
        PREFIX_AS(texture)
                    INPUT_STR(src, texture_src, p->source)
        SUFFIX_AS
    }
}