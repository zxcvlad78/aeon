#include "Systems.hpp"
#include "Components.hpp"

void input_system(entt::registry& registry) {
    auto view = registry.view<MoveDirection>();

    view.each([](MoveDirection& move_dir) {
        move_dir.up = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
        move_dir.down = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
        move_dir.left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
        move_dir.right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
    });
}

void movement_system(entt::registry& registry, float dt) {
    auto view = registry.view<Position, Velocity, MoveSpeed, MoveDirection>();

    view.each([dt](Position& pos, Velocity& vel, MoveSpeed& movespeed, MoveDirection& move_dir) {
        vel.x = 0.0f;
        vel.y = 0.0f;
        
        if (move_dir.up) { vel.y -= movespeed.value; }
        if (move_dir.down) { vel.y += movespeed.value; }
        if (move_dir.left) { vel.x -= movespeed.value; }
        if (move_dir.right) { vel.x += movespeed.value; }

        if (vel.normalize) {
            if (vel.x != 0.0f && vel.y != 0.0f) {
                vel.x *= 0.70710678118f;
                vel.y *= 0.70710678118f;
            }
        }

        
        //std::cout << vel.x << ", " << vel.y << std::endl;
        pos.x += vel.x * dt;
        pos.y += vel.y * dt;
    });
}

void camera_system(entt::registry& registry, sf::RenderWindow& window, float dt) {
    auto view = registry.view<Position, Camera>();

    for (auto [entity, pos, camera] : view.each()) {
        sf::Vector2f current_center = camera.view.getCenter();
        sf::Vector2f target_center = {pos.x, pos.y};
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
    auto view = registry.view<Position, Sprite>();

    for (auto [entity, pos, sprite] : view.each()) {
        sprite.sprite.setPosition({pos.x, pos.y});
        window.draw(sprite.sprite);
    }
}
