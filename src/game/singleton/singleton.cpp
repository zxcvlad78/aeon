#include "singleton.hpp"
#include "../mob/Components.hpp"
#include "../../ResourceLoader.hpp"

entt::entity Singleton::spawn_projectile(
    entt::registry& registry,
    Projectile      projectile,
    Transform       transform,
    Velocity        velocity,
    std::string     atlas_path,
    std::string     spritesheet_path)
{
    auto projectile_entity = registry.create();
        registry.emplace<ZIndex>(projectile_entity, 2);
        registry.emplace<Projectile>(projectile_entity, projectile);
        registry.emplace<Velocity>(projectile_entity, velocity);
        registry.emplace<Transform>(projectile_entity, transform);

    Sprite projectile_sprite(resourceloader.load<sf::Texture, sf::TextureLoader>(atlas_path));
        projectile_sprite.center = true;
        registry.emplace<Sprite>(projectile_entity, projectile_sprite);
    
    SpriteAnimation projectile_sprite_anim;
        projectile_sprite_anim.spritesheet = resourceloader.load<Spritesheet::Resource, Spritesheet::Loader>(spritesheet_path);
        projectile_sprite_anim.play("idle");
        registry.emplace<SpriteAnimation>(projectile_entity, projectile_sprite_anim);

    Hitbox projectile_hitbox;
        FrameData sprite_fd = projectile_sprite_anim.current_animation->frames.front();
        projectile_hitbox.size = {
            static_cast<float>(sprite_fd.w),
            static_cast<float>(sprite_fd.h)
        };
        
        projectile_hitbox.offset = {-projectile_hitbox.size.x / 2.f, -projectile_hitbox.size.y / 2.f};
        
        registry.emplace<Hitbox>(projectile_entity, projectile_hitbox);
    
    return projectile_entity;
}

entt::entity Singleton::spawn_enemy(
    entt::registry& registry,
    std::string projectile_atlas_path,
    std::string projectile_spritesheet_path)
{
    auto entity = registry.create();
    registry.emplace<ZIndex>(entity, 1);
    registry.emplace<Transform>(entity);
    registry.emplace<Velocity>(entity, 0.f, 0.f, true);
    registry.emplace<SpriteAnimationControl>(entity);
    
    registry.emplace<Health>(entity, 100.f, 100.f);
    registry.emplace<MoveSpeed>(entity, 100.0f);
    
    registry.emplace<Mob>(entity);

    Attack attack;
        registry.emplace<Attack>(entity, attack);

    MobAttackRanged mob_attack_ranged;
        mob_attack_ranged.projectile = Projectile();
        mob_attack_ranged.initial_velocity = Velocity(100.f, 100.f, true);
        mob_attack_ranged.projectile_atlas_path = projectile_atlas_path;
        mob_attack_ranged.projectile_spritesheet_path = projectile_spritesheet_path;

        registry.emplace<MobAttackRanged>(entity, mob_attack_ranged);

    Hitbox hitbox;
        hitbox.size = {16.f, 32.f};
        hitbox.offset = {-hitbox.size.x / 2.f, -hitbox.size.y / 2.f};
        registry.emplace<Hitbox>(entity, hitbox);

    Sprite sprite(resourceloader.load<sf::Texture, sf::TextureLoader>("res/textures/zloipacan/atlas.png"));
        sprite.offset = {-8.f, -16.f};
        registry.emplace<Sprite>(entity, sprite);
    
    SpriteAnimation sprite_anim;
        sprite_anim.spritesheet = resourceloader.load<Spritesheet::Resource, Spritesheet::Loader>("res/textures/zloipacan/spritesheet.json");
        registry.emplace<SpriteAnimation>(entity, sprite_anim);

    HealthBar healthbar;
        healthbar.offset = sprite.offset + sf::Vector2f(-4.f, -6.f);
        healthbar.size = {24.f, 3.5f};
        healthbar.color = sf::Color::Black;
        healthbar.color_empty = sf::Color::Red;
        healthbar.color_full = sf::Color::Green;
        
        registry.emplace<HealthBar>(entity, healthbar);

    return entity;
}
