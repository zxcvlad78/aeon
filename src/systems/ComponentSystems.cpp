#include "ComponentSystems.hpp"
#include "Components.hpp"

#include "../ResourceLoader.hpp"
#include "../SoundPlayer.hpp"

#include "../game/singleton/singleton.hpp"
#include "../utils/math.hpp"
#include "../console/Console.hpp"

#include "../game/sprite/Components.hpp"
#include "../utils/spatial_hashing.hpp"

extern bool enable_render_system;

void player_input_system(entt::registry& registry, sf::RenderWindow& window) {
    auto view = registry.view<PlayerInput, MoveSpeed, Velocity>();

    for (auto [entity, player_input, movespeed, velocity] : view.each()) {
        velocity.x = 0.0f;
        velocity.y = 0.0f;

        if (Console::get_instance().is_visible()) { continue; }
        
        velocity.y -= sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
        velocity.y += sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
        velocity.x -= sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
        velocity.x += sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
        
        velocity.x *= movespeed.value;
        velocity.y *= movespeed.value;

        if (velocity.x == 0.0f && velocity.y == 0.0f) {
            continue;
        }

        if (velocity.normalize) {
            if (velocity.x != 0.0f && velocity.y != 0.0f) {
                velocity.x *= 0.70710678118f;
                velocity.y *= 0.70710678118f;
            }
        }
    }
}

void attack_system_manager_handler(entt::registry& registry, float dt) {
    auto view = registry.view<Attack, Transform, PlayerInput>();


    for (auto [entity, attack, transform, player_input] : view.each()) {
        if (attack.in_cooldown()) {
            attack.cooldown -= dt;
            continue;
        }

        if (attack.spawn_func == nullptr) continue;

        sf::Vector2f dir { };

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            dir.y = -1.f;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            dir.y = 1.f;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            dir.x = -1.f;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            dir.x = 1.f;
        }

        if (dir.x == 0.0f && dir.y == 0.0f) continue;

        auto projectile = attack.spawn_func(registry, entity); {
            if (auto* t = registry.try_get<Transform>(projectile)) {
                t->position = transform.position;
            }
            if (auto* p = registry.try_get<Projectile>(projectile)) {
                if (auto* v = registry.try_get<Velocity>(projectile)) {
                    v->x = (dir.x * p->speed);
                    v->y = (dir.y * p->speed);
                }
            }
        }

        attack.cooldown = attack.interval;
    }
}

void glue_system(entt::registry& registry) {
    auto view = registry.view<Transform, GluedTo>();

    for (auto [entity, transform, glued_to] : view.each()) {
        if (glued_to.entity == entt::null) {
            continue;
        }

        Transform* target_transform = registry.try_get<Transform>(glued_to.entity);
        if (!target_transform) {
            continue;
        }

        transform.position = target_transform->position;
    }
}

void movement_system(entt::registry& registry, float dt) {
    auto view = registry.view<Transform, Velocity>();

    for (auto [entity, transform, vel] : view.each()) {
        //std::cout << vel.x << ", " << vel.y << std::endl;
        transform.position.x += vel.x * dt;
        transform.position.y += vel.y * dt;
    }
}

void sprite_animation_control_system(entt::registry& registry) {
    auto view = registry.view<SpriteAnimation, SpriteAnimationControl, Velocity>();
    
    for (auto [entity, sprite_anim, control, vel] : view.each()) {
        if (!sprite_anim.spritesheet) continue;

        std::string target_anim_name = control.idle_anim;

        if (vel.y < 0.0f)      target_anim_name = control.up_anim;
        else if (vel.y > 0.0f) target_anim_name = control.down_anim;
        else if (vel.x > 0.0f) target_anim_name = control.right_anim;
        else if (vel.x < 0.0f) target_anim_name = control.left_anim;

        auto it = sprite_anim.spritesheet->animations.find(target_anim_name);
        if (it != sprite_anim.spritesheet->animations.end()) {
            const Animation::Resource* target_animation_ptr = &it->second;

            if (sprite_anim.current_animation != target_animation_ptr) {
                sprite_anim.play(target_anim_name);
            }
        }
    }
}


void render_healthbar(entt::registry& registry, sf::RenderWindow& window) {
    auto view = registry.view<HealthBar, Transform, Health>();
    
    for (auto [entity, healthbar, transform, health] : view.each()) {
        sf::Vector2f pos = transform.position + healthbar.offset;

        sf::RectangleShape color_bar(sf::Vector2f(healthbar.size.x, healthbar.size.y));
        color_bar.setPosition(pos);
        color_bar.setFillColor(healthbar.color_empty);
        color_bar.setOutlineColor(healthbar.outline_color);
        color_bar.setOutlineThickness(healthbar.outline_thickness);
        window.draw(color_bar);
    
        float max_health = health.get_max_value();
        float current_health = health.get_value();

        float ratio = max_health > 0.0f ? (current_health / max_health) : 0.0f;
        float width = healthbar.size.x * ratio;
        
        sf::RectangleShape full_bar(sf::Vector2f(width, healthbar.size.y));
        full_bar.setPosition(pos);
        full_bar.setFillColor(ratio < 0.5f ? healthbar.color_empty : healthbar.color_full);

        window.draw(full_bar);

    }
}



void projectile_system(entt::registry& registry, float dt) {
    const float CELL_SIZE = 64.0f;
    SpatialHash spatial_hash(CELL_SIZE);

    auto hitbox_view = registry.view<Transform, Hitbox>();
    for (auto [entity, transform, hitbox] : hitbox_view.each()) {
        sf::FloatRect aabb(
            {transform.position.x + hitbox.offset.x, transform.position.y + hitbox.offset.y},
            hitbox.size
        );
        spatial_hash.insert(entity, aabb);
    }

    auto view1 = registry.view<Transform, Projectile, Hitbox>();
    std::vector<entt::entity> to_destroy;

    for (auto [entity1, transform1, projectile1, hitbox1] : view1.each()) {
        projectile1.time_elapsed += dt;
        if (projectile1.time_elapsed >= projectile1.lifetime) {
            to_destroy.push_back(entity1);
            continue;
        }

        if (!collision_enabled) { continue; }
        if (projectile1.damaged_entity != entt::null) { continue; }

        sf::FloatRect aabb1(
            {transform1.position.x + hitbox1.offset.x, transform1.position.y + hitbox1.offset.y},
            hitbox1.size
        );

        auto candidates = spatial_hash.query(aabb1);

        for (auto entity2 : candidates) {
            if (entity2 == entity1 || entity2 == projectile1.source) continue;

            const auto* transform2 = registry.try_get<Transform>(entity2);
            const auto* hitbox2 = registry.try_get<Hitbox>(entity2);
            if (!transform2 || !hitbox2) continue;

            sf::FloatRect aabb2(
                {transform2->position.x + hitbox2->offset.x, transform2->position.y + hitbox2->offset.y},
                hitbox2->size
            );

            if (aabb1.findIntersection(aabb2).has_value()) {
                projectile1.damaged_entity = entity2;
                hitbox1.size = {0.f, 0.f};

                if (registry.all_of<Health>(entity2)) {
                    auto& health = registry.get<Health>(entity2);
                    health.apply_damage(projectile1.damage);
                }

                if (registry.all_of<Velocity>(entity1)) {
                    auto& vel = registry.get<Velocity>(entity1);
                    vel = {0.f, 0.f};
                }

                if (registry.all_of<SpriteAnimation>(entity1)) {
                    auto& sprite_anim = registry.get<SpriteAnimation>(entity1);
                    sprite_anim.play("death");
                    if (sprite_anim.current_animation && !sprite_anim.current_animation->frames.empty()) {
                        projectile1.time_elapsed = 0.f;
                        projectile1.lifetime = static_cast<float>(sprite_anim.current_animation->frames.size()) / sprite_anim.current_animation->fps;
                    } else {
                        projectile1.time_elapsed = projectile1.lifetime;
                    }
                }

                if (registry.all_of<Projectile>(entity2)) {
                    auto& projectile2 = registry.get<Projectile>(entity2);

                    if (registry.all_of<Velocity>(entity2)) {
                        auto& vel = registry.get<Velocity>(entity2);
                        vel = {0.f, 0.f};
                    }

                    if (registry.all_of<SpriteAnimation>(entity2)) {
                        auto& sprite_anim = registry.get<SpriteAnimation>(entity2);
                        sprite_anim.play("death");
                        if (sprite_anim.current_animation && !sprite_anim.current_animation->frames.empty()) {
                            projectile2.time_elapsed = 0.f;
                            projectile2.lifetime = static_cast<float>(sprite_anim.current_animation->frames.size()) / sprite_anim.current_animation->fps;
                        } else {
                            projectile2.time_elapsed = projectile2.lifetime;
                        }
                    }
                }

                soundplayer.play(projectile1.hit_soundbuffer, transform1.position);
                break;
            }
        }
    }

    for (auto entity : to_destroy) {
        if (registry.valid(entity)) {
            registry.destroy(entity);
        }
    }
}

void vector2_testing_system(entt::registry& registry, float dt) {
    auto view = registry.view<Vector2Testing, Transform>();
    for (auto [entity, anim, transform] : view.each()) {
        if (!anim.initialized) {
            anim.base_position = transform.position;
            anim.initialized = true;
        }
        anim.elapsed += dt;
        float angle = 2.0f * 3.14159265f * anim.frequency * anim.elapsed + anim.phase;
        float offset_x = anim.amplitude_x * std::sin(angle);
        float offset_y = anim.amplitude_y * std::sin(angle);
        transform.position = anim.base_position + sf::Vector2f{offset_x, offset_y};
    }
}

void health_system(entt::registry& registry) {
    auto view = registry.view<Health>();
    
    std::vector<entt::entity> to_destroy;

    for (auto [entity, health] : view.each()) {
        if (health.get_value() <= 0.f) {
            to_destroy.push_back(entity);
        }
    }

    for (auto entity : to_destroy) {
        if (!registry.valid(entity)) { continue; }
        registry.destroy(entity);
    }
}

void hitbox_collision_system(entt::registry& registry) {
    
}