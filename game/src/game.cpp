#include <core/entry.h>

fuse::app_config fuse::create_application() {
    fuse::app_config config{1080, 720, "Fuse2D", "0.0.1"};
    return config;
}