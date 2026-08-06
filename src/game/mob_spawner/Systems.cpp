#include "Systems.hpp"

#include <random>
#include "../../SoundPlayer.hpp"

void mob_spawner_system(entt::registry& registry, float dt) {
    auto view = registry.view<MobSpawner, Transform>();

    static std::random_device rd;
    static std::mt19937 gen(rd());

    for (auto [entity, mob_spawner, transform] : view.each()) {
        auto sprite_animation = registry.try_get<SpriteAnimation>(entity);

        if (mob_spawner.in_cooldown()) {
            mob_spawner.cooldown -= dt;
            if (sprite_animation) {
                if (!sprite_animation->is_playing && sprite_animation->current_animation->name != "idle") {
                    sprite_animation->play("idle");
                }
            }
            continue;
        }

        if (mob_spawner.spawn_func == nullptr) continue;

        mob_spawner.cooldown = mob_spawner.spawn_interval;
        entt::entity mob_enemy = mob_spawner.spawn_func(registry);
        
        if (auto t = registry.try_get<Transform>(mob_enemy)) {
            std::uniform_real_distribution<float> dis_x(-mob_spawner.spawn_range.x, mob_spawner.spawn_range.x);
            std::uniform_real_distribution<float> dis_y(-mob_spawner.spawn_range.y, mob_spawner.spawn_range.y);
            sf::Vector2f random_offset = { dis_x(gen), dis_y(gen) };
            sf::Vector2f spawn_position = transform.position + random_offset;
            t->position = spawn_position;
            soundplayer.play(mob_spawner.spawn_soundbuffer, spawn_position);
        }

        if (sprite_animation) { sprite_animation->play("spawn"); }
    }
}