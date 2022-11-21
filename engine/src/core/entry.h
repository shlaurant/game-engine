#pragma once
#include "application.h"

extern fuse::app_config fuse::create_application();

int main(int argc, char **argv){
    auto config = fuse::create_application();
    fuse::run_application(config);
    return 0;
}

#define REGISTER_SCRIPT(script) fuse::script_factory::instance()->register_script<script>(#script);