#include "ComponentSystems.hpp"
#include "Components.hpp"

void player_input_system(entt::registry& registry) {
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

void render_system(entt::registry& registry, sf::RenderWindow& window) {
    auto view = registry.view<Transform, Sprite>();

    for (auto [entity, transform, sprite] : view.each()) {
        sf::Vector2f pos = transform.position + sprite.offset;

        sprite.sprite.setPosition({pos.x, pos.y});
        sprite.sprite.setRotation({transform.rotation_degrees});
        sprite.sprite.setScale({transform.scale.x, transform.scale.y});
        window.draw(sprite.sprite);
    }

    for (auto [entity, transform, hitbox] : registry.view<Transform, Hitbox>().each()) {

    }
}


void projectile_system(entt::registry& registry, float dt) {
    auto view1 = registry.view<Transform, Projectile, Hitbox>();
    auto view2 = registry.view<Transform, Health, Hitbox>();

    for (auto [entity1, transform1, projectile1, hitbox1] : view1.each()) {
        sf::FloatRect aabb1(
            {transform1.position.x + hitbox1.offset_x, transform1.position.y + hitbox1.offset_y},
            {hitbox1.width, hitbox1.height}
        );

        for (auto [entity2, transform2, health2, hitbox2] : view2.each()) {
            sf::FloatRect aabb2(
                {transform2.position.x + hitbox2.offset_x, transform2.position.y + hitbox2.offset_y},
                {hitbox2.width, hitbox2.height}
            );

            if (aabb1.findIntersection(aabb2)) {
                health2.apply_damage(projectile1.damage); //apply damage
                std::cout << "sex" << std::endl;
            }

        }

        projectile1.time_elapsed += dt;
        if (projectile1.time_elapsed >= projectile1.lifetime) {
            registry.destroy(entity1);
        }
    }
}

void hitbox_collision_system(entt::registry& registry) {
    
}