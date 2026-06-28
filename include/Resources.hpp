#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <vector>
#include <map>
#include <string>

#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"

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

        std::shared_ptr<Texture> operator()(const std::string& path) const {
            auto texture = std::make_shared<Texture>();
            if (!texture->loadFromFile(path)) {
                return nullptr;
            }
            return texture;
        }
    };

    struct SoundBufferLoader {
        using result_type = std::shared_ptr<SoundBuffer>;
        
        std::shared_ptr<SoundBuffer> operator()(const std::string& path) const {
            auto sb = std::make_shared<SoundBuffer>();
            if (!sb->loadFromFile(path)) {
                return nullptr;
            }
            return sb;
        }
    };

}

namespace Animation
{
    struct Resource {
        std::string name;
        float fps = 12.f;
        bool is_looping = true;
        std::vector<FrameData> frames;
    };

    struct Loader {
        using result_type = std::shared_ptr<Resource>;
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

        std::shared_ptr<Resource> operator()(const std::string& path) const {
            json data = ::get_json_data(path);
            
            try {
                auto spritesheet = std::make_shared<Resource>();

                if (data.contains("meta") && data["meta"].is_object()) {
                    auto& meta = data["meta"];
                    spritesheet->atlas_width = meta.value("width", 1);
                    spritesheet->atlas_height = meta.value("height", 1);
                }
                if (data.contains("animations") && data["animations"].is_object()) {
                    for (auto& [anim_name, anim_data] : data["animations"].items()) {
                        Animation::Resource anim;
                        anim.name = anim_name;
                        anim.fps = anim_data.value("fps", 12.f);
                        anim.is_looping = anim_data.value("looping", true);
        
                        if (anim_data.contains("frames") && anim_data["frames"].is_array()) {
                            for (auto& frame_json : anim_data["frames"]) {
                                FrameData frame;
                                frame.x = frame_json.value("x", 0);
                                frame.y = frame_json.value("y", 0);
                                frame.w = frame_json.value("w", 0);
                                frame.h = frame_json.value("h", 0);
                                anim.frames.push_back(frame);
                            }
                        }
                        spritesheet->animations[anim_name] = std::move(anim);
                    }
                }
        
                
                return spritesheet;
        
            } catch (const std::exception& e) {
                std::cerr << "json exception" << e.what() << " in file " << path << std::endl;
                return nullptr;
            }
        }
    };

};

namespace TileSet
{
    struct Resource {
        std::shared_ptr<sf::Texture> texture;
        
        unsigned int tile_size = 16;
        int y_sort_origin = 0;
    };

    struct Loader {
        using result_type = std::shared_ptr<Resource>;;

        std::shared_ptr<Resource> operator()(const std::string& path) const {
            auto tileset = std::make_shared<Resource>();
            return tileset;
        }
    };

}
