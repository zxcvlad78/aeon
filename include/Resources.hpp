#pragma once

#include <vector>
#include <map>
#include <string>

struct FrameData {
    int x, y, w, h;
};

namespace sf
{
    struct TextureLoader {
        using result_type = Texture;

        std::shared_ptr<Texture> operator()(const std::string& path) const {
            auto texture = std::make_shared<Texture>();
            if (!texture->loadFromFile(path)) {
                return nullptr;
            }
            return texture;
        }
    };
}

namespace Animation
{
    struct Resource {
        float fps = 12.f;
        bool is_looping = true;
        std::vector<FrameData> frames;
    };

    struct Loader {
        using result_type = Resource;
    };

};

namespace Spritesheet
{
    struct Resource {
        int atlas_width = 1;
        int atlas_height = 1;
        std::unordered_map<std::string, Animation::Resource> animations;
    };

    struct Loader {
        using result_type = Resource;
    };

};

namespace TileSet
{
    struct Resource {
        sf::Texture& texture;
        
        unsigned int tile_size = 16;
        int y_sort_origin = 0;
    };

    struct Loader {
        using result_type = Resource;
    };

}