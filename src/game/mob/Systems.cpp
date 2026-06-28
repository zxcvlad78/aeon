#include "Systems.hpp"
#include <random>

#include "../../utils/math.hpp"
#include "../../ResourceLoader.hpp"
#include "../singleton/singleton.hpp"
#include "../../SoundPlayer.hpp"

void mob_system(entt::registry& registry, float dt) {
    mob_movement_system(registry, dt);
    mob_attack_ranged_system(registry, dt);
    mob_spawner_system(registry, dt);
}

void mob_movement_system(entt::registry& registry, float dt) {

}

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

        mob_spawner.cooldown = mob_spawner.spawn_interval;
        entt::entity mob_enemy = Singleton::spawn_enemy(
            registry,
            mob_spawner.sprite_atlas_path,
            mob_spawner.sprite_spritesheet_path,
            mob_spawner.projectile_atlas_path,
            mob_spawner.projectile_spritesheet_path,
            mob_spawner.projectile_hitsound
        );

        std::uniform_real_distribution<float> dis_x(-mob_spawner.spawn_range.x, mob_spawner.spawn_range.x);
        std::uniform_real_distribution<float> dis_y(-mob_spawner.spawn_range.y, mob_spawner.spawn_range.y);
        sf::Vector2f random_offset = { dis_x(gen), dis_y(gen) };
        sf::Vector2f spawn_position = transform.position + random_offset;
        registry.get<Transform>(mob_enemy).position = spawn_position;

        soundplayer.play(mob_spawner.spawn_soundbuffer, spawn_position);
        if (sprite_animation) { sprite_animation->play("spawn"); }
    }
}

void mob_attack_ranged_system(entt::registry& registry, float dt) {
    auto view = registry.view<Mob, Transform, Attack, MobAttackRanged>();

    for (auto [entity, transform, mob_attack, mob_attack_ranged] : view.each()) {
        if (mob_attack.in_cooldown()) {
            mob_attack.cooldown -= dt;
            continue;
        }

        
        entt::entity enemy = MobFunc::get_nearest_enemy(registry, entity);
        if (enemy == entt::null) {
            continue;
        }
        
        mob_attack.cooldown = mob_attack.interval;
        
        Transform enemy_transform = registry.get<Transform>(enemy);

        sf::Vector2f enemy_velocity{0.0f, 0.0f};
        if (registry.all_of<Velocity>(enemy)) {
            auto& v = registry.get<Velocity>(enemy);
            enemy_velocity = sf::Vector2f(v.x, v.y);
        }


        float projectile_speed = std::hypot(mob_attack_ranged.initial_velocity.x, mob_attack_ranged.initial_velocity.y);
        float distance = std::hypot(enemy_transform.position.x - transform.position.x, enemy_transform.position.y - transform.position.y);
        float travel_time = (projectile_speed > 0.0f) ? (distance / projectile_speed) : 0.0f;

        sf::Vector2f predicted_enemy_pos = enemy_transform.position + enemy_velocity * travel_time;
        sf::Vector2f dir = Math::get_direction(transform.position, predicted_enemy_pos);


        Transform projectile_entity_transform;
            projectile_entity_transform.position = transform.position;

        Velocity projectile_entity_velocity;
            projectile_entity_velocity.x = mob_attack_ranged.initial_velocity.x * dir.x;
            projectile_entity_velocity.y = mob_attack_ranged.initial_velocity.y * dir.y;

        Singleton::spawn_projectile(
            registry,
            mob_attack_ranged.projectile,
            projectile_entity_transform,
            projectile_entity_velocity,
            mob_attack_ranged.projectile_atlas_path,
            mob_attack_ranged.projectile_spritesheet_path
        );
    }
}


namespace MobFunc {
    std::vector<entt::entity> get_enemies(entt::registry& registry, entt::entity from) {
        auto* enemy_factions = registry.try_get<EnemyFactions>(from);
        
        std::vector<entt::entity> result;

        if (!enemy_factions) {
            return result; 
        }

        auto view = registry.view<Faction>();
        for (auto [entity, faction] : view.each()) {
            if (entity == from) continue;

            auto it = std::find(enemy_factions->list.begin(), enemy_factions->list.end(), faction.name);
            if (it != enemy_factions->list.end()) {
                result.push_back(entity);
            }

        }

        return result;
    }

    std::vector<entt::entity> get_allies(entt::registry& registry, entt::entity from) {
        return std::vector<entt::entity>();
    }

    entt::entity get_nearest_enemy(entt::registry& registry, entt::entity from) {
        Transform* transform = registry.try_get<Transform>(from);
        if (!transform) {
            return entt::null;
        }

        std::vector<entt::entity> enemies = get_enemies(registry, from);
        if (enemies.empty()) {
            return entt::null;
        }

        entt::entity nearest_enemy = entt::null;
        float min_distance = FLT_MAX;

        for (auto enemy : enemies) {
            Transform* enemy_transform = registry.try_get<Transform>(enemy);
            if (!enemy_transform) {
                continue;
            }

            float dx = enemy_transform->position.x - transform->position.x;
            float dy = enemy_transform->position.y - transform->position.y;
            
            float distance_sq = (dx * dx) + (dy * dy); 

            if (distance_sq < min_distance) {
                min_distance = distance_sq;
                nearest_enemy = enemy;
            }
        }

        return nearest_enemy;
    }

    entt::entity get_nearest_ally(entt::registry& registry, entt::entity from);
}