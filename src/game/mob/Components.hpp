#include "../../../include/Components.hpp"

struct Mob {
    
};


struct MobAttackMelee {

};

struct MobAttackRanged {
    Projectile projectile;
    Velocity initial_velocity;
    std::string projectile_atlas_path;
    std::string projectile_spritesheet_path;
};

struct MobSpawner {
    std::string sprite_atlas_path;
    std::string sprite_spritesheet_path;
    std::string projectile_atlas_path;
    std::string projectile_spritesheet_path;
    std::string projectile_hitsound;
    
    float spawn_interval = 10.f;
    sf::Vector2f spawn_range = {250.f, 250.f};
    entt::resource<sf::SoundBuffer> spawn_soundbuffer;

    float cooldown = 0.f;
    bool in_cooldown() { return cooldown > 0.f; }
};