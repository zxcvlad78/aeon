#pragma once

#include "../../../include/Components.hpp"
#include "../mob/Components.hpp"
#include "../faction/Components.hpp"
#include "../ai/Components.hpp"
#include "../../ResourceLoader.hpp"

namespace packed_entity {

namespace default_projectile {
    inline entt::entity spawn(entt::registry& registry, entt::entity source = entt::null) {
        entt::entity entity = registry.create();
        auto& z_index = registry.emplace<ZIndex>(entity, 2);
        auto& projectile = registry.emplace<Projectile>(entity); {
            projectile.source = source;
            projectile.hit_soundbuffer = resourceloader.load<sf::SoundBuffer, sf::SoundBufferLoader
            >("res/audio/bulk.wav");
        }
        auto& velocity = registry.emplace<Velocity>(entity);
        auto& transform = registry.emplace<Transform>(entity);
        auto& sprite = registry.emplace<Sprite>(entity,
            resourceloader.load<sf::Texture, sf::TextureLoader
            >("res/textures/t_projectile/atlas.png")); /* texture path */ {
                sprite.center = true;
        }
        auto& sprite_animation = registry.emplace<SpriteAnimation>(entity); {
            sprite_animation.spritesheet = resourceloader.load<Spritesheet::Resource, Spritesheet::Loader
            >("res/textures/t_projectile/spritesheet.json"); // spritesheet path
            sprite_animation.play("idle");
        }
        auto& hitbox = registry.emplace<Hitbox>(entity); {
            if (sprite_animation.current_animation && !sprite_animation.current_animation->frames.empty()){
                FrameData sprite_fd = sprite_animation.current_animation->frames.front();
                hitbox.size = {
                    static_cast<float>(sprite_fd.w),
                    static_cast<float>(sprite_fd.h)
                };
            } else hitbox.size = {8.f, 8.f};
            hitbox.offset = {-hitbox.size.x / 2.f, -hitbox.size.y / 2.f};
        }

        return entity;
    }
}

namespace gad {
    inline entt::entity spawn(entt::registry& registry) {
        entt::entity entity = registry.create(); {
            registry.emplace<Mob>(entity);
            auto& z_index = registry.emplace<ZIndex>(entity, 1);
            auto& transform = registry.emplace<Transform>(entity);
            
            auto& health = registry.emplace<Health>(entity, 250.f, 250.f);
            auto& faction = registry.emplace<Faction>(entity, "gadi");

            auto& hitbox = registry.emplace<Hitbox>(entity); {
                hitbox.size = {16.f, 32.f};
                hitbox.offset = {-hitbox.size.x / 2.f, -hitbox.size.y / 2.f};
            }

            auto& sprite = registry.emplace<Sprite>(entity,
                resourceloader.load<
                sf::Texture, sf::TextureLoader
                >("res/textures/gad/atlas.png") // texture path
            ); {
                sprite.offset = {-8.f, -16.f};
            }
            auto& sprite_animation = registry.emplace<SpriteAnimation>(entity); {
                sprite_animation.spritesheet = resourceloader.load<
                Spritesheet::Resource, Spritesheet::Loader
                >("res/textures/gad/spritesheet.json"); // spritesheet path
                sprite_animation.play("idle");
            }
        }
        
        return entity;
    }
};

namespace zobi {
    inline entt::entity spawn(entt::registry& registry) {
        entt::entity entity = registry.create(); {
            registry.emplace<Mob>(entity);
            auto& z_index = registry.emplace<ZIndex>(entity, 1);
            auto& transform = registry.emplace<Transform>(entity);
            auto& velocity = registry.emplace<Velocity>(entity); {
                velocity.normalize = true;
            }
            auto& sprite_animation_control = registry.emplace<SpriteAnimationControl>(entity);
            auto& health = registry.emplace<Health>(entity, 100.f, 100.f);
            auto& move_speed = registry.emplace<MoveSpeed>(entity, 100.0f);
            auto& ai_target = registry.emplace<AIComponents::AITarget>(entity);
            auto& faction = registry.emplace<Faction>(entity, "enemy");
            auto& enemy_factions = registry.emplace<EnemyFactions>(entity); {
                enemy_factions.add("player");
                enemy_factions.add("gadi");
            }
            auto& attack = registry.emplace<Attack>(entity); 
            attack.spawn_func = packed_entity::default_projectile::spawn;
            
            auto& mob_attack_ranged = registry.emplace<MobAttackRanged>(entity); {
                Projectile projectile; {
                    projectile.damage = 10.f;
                    projectile.lifetime = 6.f;
                    projectile.source = entity;
                    projectile.hit_soundbuffer = resourceloader.load<
                        sf::SoundBuffer, sf::SoundBufferLoader
                    >("res/audio/bulk.wav"); // sound_path
                }
                mob_attack_ranged.projectile = projectile;
                mob_attack_ranged.atlas = resourceloader.load<sf::Texture, sf::TextureLoader
                    >("res/textures/t_projectile/atlas.png");
                mob_attack_ranged.spritesheet = resourceloader.load<Spritesheet::Resource, Spritesheet::Loader
                    >("res/textures/t_projectile/spritesheet.json");
            }
            auto& hitbox = registry.emplace<Hitbox>(entity); {
                hitbox.size = {16.f, 32.f};
                hitbox.offset = {-hitbox.size.x / 2.f, -hitbox.size.y / 2.f};
            }
            auto& sprite = registry.emplace<Sprite>(entity,
                resourceloader.load<
                sf::Texture, sf::TextureLoader
                >("res/textures/zloipacan/atlas.png") // texture path
            ); {
                sprite.offset = {-8.f, -16.f};
            }
            auto& sprite_animation = registry.emplace<SpriteAnimation>(entity); {
                sprite_animation.spritesheet = resourceloader.load<
                Spritesheet::Resource, Spritesheet::Loader
                >("res/textures/zloipacan/spritesheet.json"); // spritesheet path
            }
        }
        
        return entity;
    }
};

}; // namespace packed_entity