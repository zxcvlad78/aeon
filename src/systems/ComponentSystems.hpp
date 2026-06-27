#pragma once
#include "Components.hpp"

class ResourceLoader;

void player_input_system(entt::registry& registry, sf::RenderWindow& window);
void movement_system(entt::registry& registry, float dt);
void camera_system(entt::registry& registry, sf::RenderWindow& window, float dt);

void sprite_animation_control_system(entt::registry& registry);
void sprite_animation_system(entt::registry& registry, float dt);

void projectile_system(entt::registry& registry, float dt);
void sprite_system(entt::registry& registry);
void render_system(entt::registry& registry, sf::RenderWindow& window);

void render_healthbar(entt::registry& registry, sf::RenderWindow& window);


extern bool debug_hitboxes;