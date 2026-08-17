#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <vector>
#include <map>
#include <string>

#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"
#include <entt/resource/cache.hpp>

using json = nlohmann::json;

inline json get_json_data(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return nullptr;
    json j;
    file >> j;
    return j;
}

struct FrameData {
    int x, y, w, h;
};

namespace sf
{
    struct TextureLoader {
        using result_type = std::shared_ptr<Texture>;
        std::shared_ptr<Texture> operator()(const std::string& path) const;
    };

    struct SoundBufferLoader {
        using result_type = std::shared_ptr<SoundBuffer>;
        std::shared_ptr<SoundBuffer> operator()(const std::string& path) const;
    };

    struct ShaderLoader {
        using result_type = std::shared_ptr<Shader>;
        std::shared_ptr<Shader> operator()(const std::string& vertex_path, const std::string& fragment_path) const;
    };

}

namespace Animation
{
    struct Resource {
        std::string name;
        float fps = 12.f;
        bool is_looping = true;
        std::vector<FrameData> frames;

        float duration() const {
            if (fps == 0.0f) return 0.0f;
            int size = frames.size();
            if (size == 0) return 0.0f;

            return fps / size;
        }
    };

    struct Loader {
        using result_type = std::shared_ptr<Resource>;
        //
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
        using result_type = std::shared_ptr<Resource>;
        std::shared_ptr<Resource> operator()(const std::string& path) const;
    };

};


namespace TileSet
{
    struct Resource {
        entt::resource<sf::Texture> texture;
        sf::Vector2u size() {
            if (texture.handle() == nullptr) return sf::Vector2u{ 0, 0 };
            return texture->getSize();
        }
        
        unsigned short tile_size = 16;
        short y_sort_origin = 0;
    };

    struct Loader {
        using result_type = std::shared_ptr<Resource>;
        std::shared_ptr<Resource> operator()(const std::string& path) const;
    };

};
