#include "Systems.hpp"

#include "../../utils/rng.hpp"
#include "../../SoundPlayer.hpp"

void MobSpawnerSystems::update(entt::registry& registry, float dt) {
    auto view = registry.view<MobSpawner>();

    for (auto [entity, mob_spawner] : view.each()) {
        if (mob_spawner.in_cooldown()) {
            mob_spawner.cooldown -= dt;
            continue;
        }

        if (mob_spawner.spawn_func == nullptr) continue;

        registry.emplace_or_replace<MobSpawnerSpawnEvent>(entity,
            mob_spawner.spawn_func(registry)
        );
    }

    process_events(registry);
}

void MobSpawnerSystems::process_events(entt::registry& registry) {
    auto view = registry.view<MobSpawner, MobSpawnerSpawnEvent, Transform>();

    for (auto [entity, mob_spawner, event, transform] : view.each()) {
        mob_spawner.cooldown = mob_spawner.spawn_interval;

        if (auto* s_a = registry.try_get<SpriteAnimation>(entity)) {
            s_a->play("spawn", "idle");
        }

        if (auto* t = registry.try_get<Transform>(event.entity)) {
            sf::Vector2f random_offset = {
                game_rng::random<float>(mob_spawner.spawn_range.x, mob_spawner.spawn_range.y),
                game_rng::random<float>(mob_spawner.spawn_range.x, mob_spawner.spawn_range.y)
            };
            sf::Vector2f spawn_position = transform.position + random_offset;
            t->position = spawn_position;
            soundplayer.play(mob_spawner.spawn_soundbuffer, spawn_position);
        }

        registry.remove<MobSpawnerSpawnEvent>(entity);
    }
}