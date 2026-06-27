#include "Systems.hpp"
#include "../../utils/math.hpp"
#include "../../ResourceLoader.hpp"

void mob_system(entt::registry& registry, float dt) {
    mob_movement_system(registry, dt);
}

void mob_movement_system(entt::registry& registry, float dt) {

}

void mob_attack_ranged_system(entt::registry& registry, float dt) {
    auto view = registry.view<Mob, Transform, Attack, MobAttackRanged>();

    for (auto [entity, transform, mob_attack, mob_attack_ranged] : view.each()) {
        if (mob_attack.in_cooldown()) {
            mob_attack.cooldown -= dt;
            continue;
        }

        mob_attack.cooldown = mob_attack.interval;

        entt::entity enemy = MobFunc::get_nearest_enemy(registry, entity);
        if (enemy == entt::null) {
            continue;
        }

        Transform enemy_transform = registry.get<Transform>(enemy);

        {auto projectile_entity = registry.create();
            registry.emplace<Projectile>(projectile_entity, mob_attack_ranged.projectile);

            Transform projectile_entity_transform;
                projectile_entity_transform.position = transform.position;
                registry.emplace<Transform>(projectile_entity, projectile_entity_transform);

            Velocity projectile_entity_velocity;
                sf::Vector2f dir = Math::get_direction(
                    {transform.position.x, transform.position.y},
                    {enemy_transform.position.x, enemy_transform.position.y});
                projectile_entity_velocity.x = mob_attack_ranged.initial_velocity.x * dir.x;
                projectile_entity_velocity.y = mob_attack_ranged.initial_velocity.y * dir.y;
                registry.emplace<Velocity>(projectile_entity, projectile_entity_velocity);
            
            Sprite projectile_sprite(resourceloader.load<sf::Texture, sf::TextureLoader>(mob_attack_ranged.projectile_atlas_path));
                projectile_sprite.center = true;
                registry.emplace<Sprite>(projectile_entity, projectile_sprite);
            
            SpriteAnimation projectile_sprite_anim;
                projectile_sprite_anim.spritesheet = resourceloader.load<Spritesheet::Resource, Spritesheet::Loader>(mob_attack_ranged.projectile_spritesheet_path);
                projectile_sprite_anim.play("idle");
                registry.emplace<SpriteAnimation>(projectile_entity, projectile_sprite_anim);

            Hitbox projectile_hitbox;
                const sf::IntRect p_sprite_tex_rect = projectile_sprite.sprite.getTextureRect();
                projectile_hitbox.size = {
                    static_cast<float>(p_sprite_tex_rect.size.x),
                    static_cast<float>(p_sprite_tex_rect.size.y)
                };
                
                projectile_hitbox.offset = {-projectile_hitbox.size.x / 2.f, -projectile_hitbox.size.y / 2.f};
                
                registry.emplace<Hitbox>(projectile_entity, projectile_hitbox);
        }
    
    }
}



namespace MobFunc {
    std::vector<entt::entity> get_enemies(entt::registry& registry, entt::entity from) {
        auto* ally_faction = registry.try_get<Faction>(from);
        auto* enemy_factions = registry.try_get<EnemyFactions>(from);
        
        std::vector<entt::entity> result;

        if (!ally_faction || !enemy_factions) {
            return result; 
        }

        auto view = registry.view<Mob, Faction>();
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

            if (nearest_enemy == entt::null) {
                nearest_enemy = enemy;
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