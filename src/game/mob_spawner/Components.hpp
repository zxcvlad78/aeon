#pragma once
#include "../../ResourceLoader.hpp"
#include <entt/entt.hpp>

struct MobSpawner {
    std::function<entt::entity(entt::registry&)> spawn_func;
    float spawn_interval = 10.f;
    sf::Vector2f spawn_range = {250.f, 250.f};
    entt::resource<sf::SoundBuffer> spawn_soundbuffer;

    float cooldown = 0.f;
    bool in_cooldown() { return cooldown > 0.f; }
};

struct MobSpawnerSpawnEvent {
    entt::entity entity;
};