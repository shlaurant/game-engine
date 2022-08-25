#pragma once

#include "pch.h"

namespace fuse::math {
    FUSE_INLINE int random(int min, int max) {
        static bool first = true;
        if (first) {
            srand(time(nullptr));
            first = false;
        }
        return min + rand() % (max + 1 - min);
    }
}