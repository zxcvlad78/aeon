#pragma once
#include <entt/entt.hpp>
#include "Resources.hpp"

#include <functional>
#include <unordered_set>

struct Transform {
    sf::Vector2f position;
    sf::Angle rotation_degrees;
    sf::Vector2f scale = {1.f, 1.f};

    Transform& operator=(const Transform& t) {
        if (this != &t) {
            position = t.position;
            rotation_degrees = t.rotation_degrees;
            scale = t.scale;
        }
        return *this;
    }

    Transform& operator=(const Transform* t) {
        if (t != nullptr && this != t) {
            position = t->position;
            rotation_degrees = t->rotation_degrees;
            scale = t->scale;
        }
        return *this;
    }
};

struct Velocity {
    float x = 0.0f;
    float y = 0.0f;
    bool normalize = true;
};

struct GluedTo {
    entt::entity entity = entt::null;
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

    float get_value() const { return value; }
    float get_max_value() const { return max_value; }

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
    float radius = 0.0f;
    //bool center = false;
};

struct Projectile {
    float damage = 10.f;
    float lifetime = 10.f;

    float speed = 100.f;

    entt::entity source = entt::null;
    entt::resource<sf::SoundBuffer> hit_soundbuffer;

    entt::entity damaged_entity = entt::null;
    float time_elapsed = 0.f;
};

struct Attack {
    float interval = 1.f;
    std::function<entt::entity(entt::registry&, entt::entity)> spawn_func;
    
    float cooldown = 1.f;
    bool in_cooldown() { return cooldown > 0.f; }
};

struct Vector2Testing {
    float elapsed = 0.0f;
    float amplitude_x = 10.f;
    float amplitude_y = 10.f;
    float frequency = 1.0f;
    float phase = 0.0f;
    sf::Vector2f base_position;
    bool initialized = false;
};


struct PlayerInput {
    bool is_current = true;
};

struct Explosion {
    float radius = 1.0f;
    float damage = 50.0f;
    float lifetime = 1.0f;

    entt::resource<sf::SoundBuffer> soundbuffer;

    float time_elapsed = 0.0f;
    std::unordered_set<entt::entity> damaged_entities;
};

struct SpriteAnimationControl {
    std::string idle_anim = "idle";
    std::string up_anim = "walk_up";
    std::string down_anim = "walk_down";
    std::string left_anim = "walk_left";
    std::string right_anim = "walk_right";
};

struct Invulnerable { };