#include "ResourceLoader.hpp" 


namespace sf
{
    std::shared_ptr<Texture> TextureLoader::operator()(const std::string& path) const {
        auto texture = std::make_shared<Texture>();
        if (!texture->loadFromFile(path)) {
            return nullptr;
        }
        return texture;
    }

    std::shared_ptr<SoundBuffer> SoundBufferLoader::operator()(const std::string& path) const {
        auto sb = std::make_shared<SoundBuffer>();
        if (!sb->loadFromFile(path)) {
            return nullptr;
        }
        return sb;
        
    };

    std::shared_ptr<Shader> ShaderLoader::operator()(const std::string& vertex_path, const std::string& fragment_path) const {
        auto shader = std::make_shared<Shader>();
        if (!shader->loadFromFile(vertex_path, fragment_path)) {
            return nullptr;
        }
        return shader;
    }

}

namespace Animation {
    
};

namespace Spritesheet {
    std::shared_ptr<Resource> Loader::operator()(const std::string& path) const {
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


namespace TileSet
{
    std::shared_ptr<Resource> Loader::operator()(const std::string& path) const {
        json data = ::get_json_data(path);
        if (data.is_null()) {
            std::cerr << "TileSet: failed to parse JSON from " << path << std::endl;
            return nullptr;
        }

        try {
            auto tileset = std::make_shared<Resource>();

            if (data.contains("texture_path") && data["texture_path"].is_string()) {
                tileset->texture = resourceloader.load<sf::Texture, sf::TextureLoader>(data["texture_path"].get<std::string>());
            } else {
                std::cerr << "TileSet: missing 'texture_path' field in " << path << std::endl;
                return nullptr;
            }

            if (data.contains("tile_size") && data["tile_size"].is_number_unsigned()) {
                tileset->tile_size = data["tile_size"].get<unsigned short>();
            } else {
                std::cerr << "TileSet: missing 'tile_size' field in " << path << std::endl;
                return nullptr;
            }

            if (data.contains("y_sort_origin") && data["y_sort_origin"].is_number_integer()) {
                tileset->y_sort_origin = data["y_sort_origin"].get<short>();
            } else {
                tileset->y_sort_origin = 0;
            }

            return tileset;

        } catch (const std::exception& e) {
            std::cerr << "TileSet: exception while loading " << path << ": " << e.what() << std::endl;
            return nullptr;
        }
    }
}