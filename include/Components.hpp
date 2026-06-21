#pragma once

#include <SFML/Graphics.hpp>
#include "Resources.hpp"
#include <vector>
#include <map>
#include <string>

struct Position {
    float x = 0.0f;
    float y = 0.0f;
};

struct Velocity {
    float x = 0.0f;
    float y = 0.0f;
    bool normalize = true;
};

struct MoveSpeed {
    float value = 10.0f;
};

struct MoveDirection {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
};

struct Camera {
    sf::View view;
    float zoom = 1.0f;
    bool smooth = true;
    bool follow = true;
};

struct Sprite {
    sf::Sprite sprite;

    Sprite(const sf::Texture& texture) : sprite(texture) {}
};

struct SpriteAnimation {
    const SpritesheetResource* spritesheet = nullptr;
    const AnimationResource* current_animation = nullptr;
    
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

struct SpriteAnimationControl {
    std::string idle_anim = "idle";
    std::string up_anim = "walk_up";
    std::string down_anim = "walk_down";
    std::string left_anim = "walk_left";
    std::string right_anim = "walk_right";
};

struct TileMap {

};