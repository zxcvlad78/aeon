#pragma once
#include "Components.hpp"

class ResourceLoader;

void glue_system(entt::registry& registry);
void player_input_system(entt::registry& registry, sf::RenderWindow& window);
void movement_system(entt::registry& registry, float dt);
void vector2_testing_system(entt::registry& registry, float dt);
void explosion_sus(entt::registry& registry, float dt);

void sprite_animation_control_system(entt::registry& registry);

void attack_system_manager_handler(entt::registry& registry, float dt);

void projectile_system(entt::registry& registry, float dt);

void health_system(entt::registry& registry);

extern bool collision_enabled;