#pragma once

#include "Systems.hpp"

#include "../ai/Components.hpp"

#include "../../utils/math.hpp"
#include "../../ResourceLoader.hpp"
#include "../singleton/singleton.hpp"
#include "../../SoundPlayer.hpp"

#include "../packed_entity/general.h"

void MobSystems::update(entt::registry& registry, float dt) {
    if (!enabled) return;
    movement(registry, dt);
    attack_ranged(registry, dt);
}

void MobSystems::movement(entt::registry& registry, float dt) {
    auto view_ranged = registry.view<MoveSpeed, Transform, Velocity, AIComponents::AITarget, Mob>();
    for (auto [entity, ms, transform, vel, ai_target] : view_ranged.each()) {
        if (ai_target.get_entity() == entt::null) {
            vel = {0.f, 0.f};
            continue;
        }

        Transform* enemy_transform = registry.try_get<Transform>(ai_target.get_entity());
        if (!enemy_transform) {
            vel = {0.f, 0.f};
            continue;
        }

        float distance = Math::get_distance(transform.position, enemy_transform->position);
        if (distance <= 150.f) {
            vel = {0.f, 0.f};
            continue;
        }

        sf::Vector2f direction = Math::get_direction(transform.position, enemy_transform->position);
        vel = {
            direction.x * ms.value,
            direction.y * ms.value
        };
    }
}


void MobSystems::attack_ranged(entt::registry& registry, float dt) {
    auto view = registry.view<Mob, Transform, Attack, AIComponents::AITarget, MobAttackRanged>();

    for (auto [entity, transform, mob_attack, ai_target] : view.each()) {
        if (mob_attack.in_cooldown()) {
            mob_attack.cooldown -= dt;
            continue;
        }

        if (ai_target.should_pick()) {
            ai_target.pick(MobFunc::get_nearest_enemy(registry, entity));
        }

        entt::entity enemy = ai_target.get_entity();
        if (enemy == entt::null) {
            continue;
        }
        
        mob_attack.cooldown = mob_attack.interval;
        
        Transform* enemy_transform = registry.try_get<Transform>(enemy);
        if (!enemy_transform) {
            return;
        }
        
        sf::Vector2f enemy_velocity{0.0f, 0.0f};
        if (registry.all_of<Velocity>(enemy)) {
            auto& v = registry.get<Velocity>(enemy);
            enemy_velocity = sf::Vector2f(v.x, v.y);
        }
        
        auto projectile_entity = mob_attack.spawn_func(registry, entity); {
            if (auto* t = registry.try_get<Transform>(projectile_entity)) {
                t->position = transform.position;
            }
            
            if (auto* projectile = registry.try_get<Projectile>(projectile_entity)) {
                if (auto* v = registry.try_get<Velocity>(projectile_entity)) {
                    float distance = std::hypot(enemy_transform->position.x - transform.position.x, enemy_transform->position.y - transform.position.y);
                    float travel_time = distance;
                    sf::Vector2f predicted_enemy_pos = enemy_transform->position + enemy_velocity * travel_time;
                    sf::Vector2f dir = Math::get_direction(transform.position, predicted_enemy_pos);
                    v->x = projectile->speed * dir.x;
                    v->y = projectile->speed * dir.y;
                }
            }
        }
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

            auto it = std::find(enemy_factions->list.begin(), enemy_factions->list.end(), faction.id);
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