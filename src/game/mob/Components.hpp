#pragma once
#include "../../../include/Components.hpp"
#include "../../ResourceLoader.hpp"

struct Mob {
    
};


struct MobAttackMelee {

};

struct MobAttackRanged {
    Projectile projectile;
    Velocity initial_velocity;
    entt::resource<sf::Texture> atlas;
    entt::resource<Spritesheet::Resource> spritesheet;
    
};
