#pragma once
#include <entt/entt.hpp>
#include "Resources.hpp"


struct Transform {
    sf::Vector2f position;
    sf::Angle rotation_degrees;
    sf::Vector2f scale = {1.f, 1.f};
};

struct Velocity {
    float x = 0.0f;
    float y = 0.0f;
    bool normalize = true;
};

struct MoveSpeed {
    float value = 10.f;
};

struct Health {
private:
    float value = 100.f;
    float max_value = 100.f;

public:
    Health(float v, float mv) { value = v, max_value = mv; }


    void set_max_value(float new_max_value) {
        max_value = new_max_value;
        if (max_value < 0.f) {
            max_value = 0.f;
        }
    }
    float get_max_value() { return max_value; }

    void set_value(float new_value) {
        value = new_value;
        if (value > max_value) {
            value = max_value;
        } else if (value < 0.f) {
            value = 0.f;
            //death
        }
    }

    float get_value() { return value; }

    void apply_damage(float damage) {
        set_value(value - damage);
    }

    void apply_heal(float heal) {
        set_value(value + heal);
    }
};


struct Hitbox {
    sf::Vector2f size;
    sf::Vector2f offset;
    //bool center = false;
};

struct Projectile {
    float damage = 10.f;
    float lifetime = 10.f;
    entt::entity source = entt::null;

    entt::entity damaged_entity = entt::null;
    float time_elapsed = 0.f;
};


struct Camera {
    sf::View view;
    float zoom = 1.0f;
    bool smooth = true;
    bool follow = true;
};


struct ZIndex {
    int value = 0;
};

struct Sprite {
    sf::Sprite sprite;
    sf::Vector2f offset;
    bool center = false;

    Sprite(entt::resource<sf::Texture> texture) : sprite(*texture) {
        
    }
};

struct SpriteAnimation {
    entt::resource<Spritesheet::Resource> spritesheet;
    const Animation::Resource* current_animation = nullptr;
    
    unsigned int current_frame_idx = 0;
    float time_accumulator = 0.0f;
    bool is_playing = true;

    void play(const std::string& animation_name) {
        if (spritesheet && spritesheet->animations.contains(animation_name)) {
            current_animation = &spritesheet->animations.at(animation_name);
            current_frame_idx = 0;
            time_accumulator = 0.0f;
        }
    }
};

struct PlayerInput {
    bool is_current = true;
};

struct SpriteAnimationControl {
    std::string idle_anim = "idle";
    std::string up_anim = "walk_up";
    std::string down_anim = "walk_down";
    std::string left_anim = "walk_left";
    std::string right_anim = "walk_right";
};

struct TileMap {

};

struct Enemy {

};