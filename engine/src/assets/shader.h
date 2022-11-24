#pragma once

#include "asset.h"

namespace fuse {
    struct shader : public asset {
        std::string filename;
    };
}