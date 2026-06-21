#include "ResourceLoader.hpp"

#include <fstream>
#include "nlohmann/json.hpp"
#include <iostream>

using json = nlohmann::json;

const sf::Texture* ResourceLoader::load_texture(const std::string& path) {
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second.get();
    }

    
    sf::Texture texture;
    texture.setSmooth(false);
    if (texture.loadFromFile(path)) {
        m_textures[path] = std::make_unique<sf::Texture>(std::move(texture));
        return m_textures[path].get();
    }

    return nullptr;
}

const sf::SoundBuffer* ResourceLoader::load_sound(const std::string& path) {
    auto it = m_sounds.find(path);
    if (it != m_sounds.end()) return it->second.get();

    sf::SoundBuffer buffer;
    if (buffer.loadFromFile(path)) {
        m_sounds[path] = std::make_unique<sf::SoundBuffer>(std::move(buffer));
        return m_sounds[path].get();
    }
    return nullptr;
}

const SpritesheetResource* ResourceLoader::load_spritesheet(const std::string& path) {
    auto it = m_spritesheets.find(path);
    if (it != m_spritesheets.end()) {
        return it->second.get();
    }

    std::ifstream file(path);
    if (!file.is_open()) return nullptr;

    try {
        json data;
        file >> data;

        SpritesheetResource spritesheet;

        if (data.contains("meta") && data["meta"].is_object()) {
            auto& meta = data["meta"];
            spritesheet.atlas_width = meta.value("width", 1);
            spritesheet.atlas_height = meta.value("height", 1);
        }

        if (data.contains("animations") && data["animations"].is_object()) {
            for (auto& [anim_name, anim_data] : data["animations"].items()) {
                AnimationResource anim;
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
                spritesheet.animations[anim_name] = std::move(anim);
            }
        }

        
        m_spritesheets[path] = std::make_unique<SpritesheetResource>(std::move(spritesheet));
        return m_spritesheets[path].get();

    } catch (const std::exception& e) {
        std::cerr << "JSON Exception" << e.what() << " in file " << path << std::endl;
        return nullptr;
    }
}
