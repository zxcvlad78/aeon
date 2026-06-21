#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include "Resources.hpp"

#include <vector>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <memory>


class ResourceLoader {
private:
    std::tuple<
        entt::resource_cache<sf::Texture, sf::TextureLoader>,
        entt::resource_cache<Spritesheet::Resource, Spritesheet::Loader>,
        entt::resource_cache<TileSet::Resource, TileSet::Loader>
    > m_caches;

public:
    ResourceLoader() = default;
    ~ResourceLoader() = default;
    ResourceLoader(const ResourceLoader&) = delete;
    ResourceLoader& operator=(const ResourceLoader&) = delete;

    const sf::Texture* load_texture(const std::string& path);
    const sf::SoundBuffer* load_sound(const std::string& path);
    const SpritesheetResource* load_spritesheet(const std::string& path);

    const sf::Texture& get_texture(const std::string& path) const {
        auto it = m_textures.find(path);
        if (it == m_textures.end()) throw std::runtime_error("Texture not found: " + path);
        return *it->second;
    }

    const sf::SoundBuffer& get_sound(const std::string& path) const {
        auto it = m_sounds.find(path);
        if (it == m_sounds.end()) throw std::runtime_error("Audio not found: " + path);
        return *it->second;
    }

    const SpritesheetResource& get_spritesheet(const std::string& path) const {
        auto it = m_spritesheets.find(path);
        if (it == m_spritesheets.end()) throw std::runtime_error("Spritesheet not found: " + path);
        return *it->second;
    }
};