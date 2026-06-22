#pragma once

#include "Resources.hpp"
#include <entt/resource/cache.hpp>

class ResourceLoader {
private:
    std::tuple<
        entt::resource_cache<sf::Texture, sf::TextureLoader>,
        entt::resource_cache<sf::SoundBuffer, sf::SoundBufferLoader>,
        entt::resource_cache<Spritesheet::Resource, Spritesheet::Loader>,
        entt::resource_cache<TileSet::Resource, TileSet::Loader>
    > m_caches;

    template <typename T, typename Loader>
    auto& get_cache() {
        return std::get<entt::resource_cache<T, Loader>>(m_caches);
    }

public:
    ResourceLoader() = default;
    ~ResourceLoader() = default;
    ResourceLoader(const ResourceLoader&) = delete;
    ResourceLoader& operator=(const ResourceLoader&) = delete;

    template <typename T, typename Loader, typename ...Args>
    entt::resource<T> load(const std::string path, Args&&... args) {
        auto id = entt::hashed_string{path.c_str()};
        auto& cache = get_cache<T, Loader>();

        auto result = cache.load(id, std::forward<Args>(args)..., path);
        return result.first->second;
    }

    template<typename T, typename Loader>
    entt::resource<T> get(const std::string& path) {
        auto id = entt::hashed_string{path.c_str()};
        auto& cache = get_cache<T, Loader>();
        return cache.handle(id);
    }

};