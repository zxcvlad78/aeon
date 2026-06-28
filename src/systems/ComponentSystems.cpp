#include "ComponentSystems.hpp"
#include "Components.hpp"

#include "../ResourceLoader.hpp"
#include "../SoundPlayer.hpp"

#include "../game/singleton/singleton.hpp"
#include "../utils/math.hpp"

void player_input_system(entt::registry& registry, sf::RenderWindow& window) {
    auto view = registry.view<PlayerInput, MoveSpeed, Velocity>();

    for (auto [entity, player_input, movespeed, velocity] : view.each()) {
        velocity.x = 0.0f;
        velocity.y = 0.0f;
        
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


void movement_system(entt::registry& registry, float dt) {
    auto view = registry.view<Transform, Velocity>();

    for (auto [entity, transform, vel] : view.each()) {
        //std::cout << vel.x << ", " << vel.y << std::endl;
        transform.position.x += vel.x * dt;
        transform.position.y += vel.y * dt;
    }
}

void camera_system(entt::registry& registry, sf::RenderWindow& window, float dt) {
    auto view = registry.view<Transform, Camera>();

    for (auto [entity, transform, camera] : view.each()) {
        sf::Vector2f current_center = camera.view.getCenter();
        sf::Vector2f target_center = {transform.position.x, transform.position.y};
        sf::Vector2f lex = target_center;

        if (camera.smooth) {
            lex.x = current_center.x + (target_center.x - current_center.x) * 5.0f * dt;
            lex.y = current_center.y + (target_center.y - current_center.y) * 5.0f * dt;
        }


        camera.view.setCenter(lex);
        sf::Listener::setPosition({lex.x, lex.y, 0.f});
    }

    auto camera_view = registry.view<Camera>();
    if (!camera_view.empty()) {
        auto camera_entity = camera_view.front();
        auto& cam = camera_view.get<Camera>(camera_entity);
        window.setView(cam.view);
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

void sprite_animation_system(entt::registry& registry, float dt) {
    auto view = registry.view<SpriteAnimation, Sprite>();
   
    for (auto [entity, sprite_anim, sprite] : view.each()) {
        if (!sprite_anim.is_playing || !sprite_anim.current_animation || sprite_anim.current_animation->frames.empty()) {
            continue;
        }

        
        sprite_anim.time_accumulator += dt;
        float frame_duration = 1.0f / sprite_anim.current_animation->fps;

        while (sprite_anim.time_accumulator >= frame_duration) {
            sprite_anim.time_accumulator -= frame_duration;
            size_t next_frame = sprite_anim.current_frame_idx + 1;

            if (next_frame >= sprite_anim.current_animation->frames.size()) {
                if (sprite_anim.current_animation->is_looping) {
                    sprite_anim.current_frame_idx = 0;
                } else {
                    sprite_anim.is_playing = false;
                    break;
                }
            } else {
                sprite_anim.current_frame_idx = next_frame;
            }
        }

        const FrameData& frame = sprite_anim.current_animation->frames[sprite_anim.current_frame_idx];
        sprite.sprite.setTextureRect(sf::IntRect({frame.x, frame.y}, {frame.w, frame.h}));

    }
}

void sprite_system(entt::registry& registry, sf::RenderWindow& window) {
    struct Renderable {
        sf::Sprite* sprite;
        sf::Vector2f position;
        sf::Vector2f offset;
        sf::Angle rotation;
        sf::Vector2f scale;
        int z_index;
    };

    std::vector<Renderable> renderables;

    for (auto [entity, transform, sprite] : registry.view<Transform, Sprite>().each()) {
        if (sprite.center) {
            auto tex_rect = sprite.sprite.getTextureRect();
            sprite.offset = {
                -static_cast<float>(tex_rect.size.x) / 2.f,
                -static_cast<float>(tex_rect.size.y) / 2.f
            };
        }

        int z_index = 0;
        
        if (registry.all_of<ZIndex>(entity)) {
            z_index = registry.get<ZIndex>(entity).value;
        }

        Renderable renderable;
        renderable.sprite = &sprite.sprite;
        renderable.position = transform.position;
        renderable.offset = sprite.offset;
        renderable.rotation = transform.rotation_degrees;
        renderable.scale = transform.scale;
        renderable.z_index = z_index;

        renderables.push_back(renderable);
    }

    std::sort(renderables.begin(), renderables.end(), 
        [](const Renderable& a, const Renderable& b) {
            return a.z_index < b.z_index;
        });

    for (const auto& renderable : renderables) {
        sf::Vector2f pos = renderable.position + renderable.offset;
        renderable.sprite->setPosition({pos.x, pos.y});
        renderable.sprite->setRotation({renderable.rotation});
        renderable.sprite->setScale({renderable.scale.x, renderable.scale.y});
        
        window.draw(*renderable.sprite);
    }
}

void render_healthbar(entt::registry& registry, sf::RenderWindow& window) {
    auto view = registry.view<HealthBar, Transform, Health>();
    
    for (auto [entity, healthbar, transform, health] : view.each()) {
        sf::Vector2f pos = transform.position + healthbar.offset;

        sf::RectangleShape color_bar(sf::Vector2f(healthbar.size.x, healthbar.size.y));
        color_bar.setPosition(pos);
        color_bar.setFillColor(healthbar.color);
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

void render_system(entt::registry& registry, sf::RenderWindow& window) {
    sprite_system(registry, window);
    render_healthbar(registry, window);

    if (debug_hitboxes) {
        for (auto [entity, transform, hitbox] : registry.view<Transform, Hitbox>().each()) {
            sf::Vector2f pos = transform.position + hitbox.offset;
    
            sf::RectangleShape color_bar(hitbox.size);
            color_bar.setPosition(pos);
            color_bar.setFillColor(sf::Color(255, 0, 0, 126.f));
            window.draw(color_bar);
        }
    }
}


void projectile_system(entt::registry& registry, float dt) {
    auto view1 = registry.view<Transform, Projectile, Hitbox>();
    auto view2 = registry.view<Transform, Hitbox>();

    std::vector<entt::entity> to_destroy;

    for (auto [entity1, transform1, projectile1, hitbox1] : view1.each()) {
        sf::FloatRect aabb1(
            {transform1.position.x + hitbox1.offset.x, transform1.position.y + hitbox1.offset.y},
            hitbox1.size
        );


        if (projectile1.damaged_entity == entt::null) {
            for (auto [entity2, transform2, hitbox2] : view2.each()) {
                if (projectile1.source == entity2 || entity1 == entity2) { continue; }
                
                sf::FloatRect aabb2(
                    {transform2.position.x + hitbox2.offset.x, transform2.position.y + hitbox2.offset.y},
                    hitbox2.size
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
                        projectile1.time_elapsed = 0.f;
                        projectile1.lifetime = sprite_anim.current_animation->frames.size() / sprite_anim.current_animation->fps;
                    }
                    
                    soundplayer.play(projectile1.hit_soundbuffer, transform1.position);
                }
            
            }
        }

        projectile1.time_elapsed += dt;
        if (projectile1.time_elapsed >= projectile1.lifetime) {
            to_destroy.push_back(entity1);
        }
    }

    for (auto entity : to_destroy) {
        registry.destroy(entity);
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
        registry.destroy(entity);
    }
}

void hitbox_collision_system(entt::registry& registry) {
    
}