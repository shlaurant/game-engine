#pragma once

namespace editor {
    struct asset {
        fuse::uuid64 asset_id;
        std::string name;
    };

    struct texture : public asset {
        std::string source;
    };

    struct animation : public asset {
        int speed;
        std::vector<fuse::uuid64> frames;
    };

    struct audio : public asset {
        std::string source;
    };

    struct font : public asset {
        int size;
        std::string source;
    };
}