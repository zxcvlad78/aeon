#pragma once

#include <Aeon.hpp>
#include "../../ResourceLoader.hpp"
#include "../singleton/singleton.hpp"
#include "../../utils/rng.hpp"

namespace packed_entity {


namespace explosion {
    inline ::entt::entity spawn(::entt::registry& registry) {
        auto entity = registry.create();
        registry.emplace<Transform>(entity);

        auto& sprite = registry.emplace<Sprite>(entity,
            resourceloader.load<
                sf::Texture,
                sf::TextureLoader
            >("res/textures/explosion/spritesheet.png")
        );
        auto& sprite_animation = registry.emplace<SpriteAnimation>(entity,
            resourceloader.load<Spritesheet::Resource, Spritesheet::Loader>("res/textures/explosion/spritesheet.json")
        ); {
            sprite_animation.play("default");
        }

        auto& exp = registry.emplace<Explosion>(entity); {
            exp.damage = 999.f;
            exp.radius = 1200.f;
            exp.lifetime = 10.f;
            exp.soundbuffer = resourceloader.load<
                sf::SoundBuffer,
                sf::SoundBufferLoader
            >("res/audio/explosion.mp3");
        }

        return entity;
    }
}

namespace default_projectile {
    inline ::entt::entity spawn(::entt::registry& registry, ::entt::entity source = ::entt::null) {
        ::entt::entity entity = registry.create();
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

namespace mo4a_projectile {
    inline ::entt::entity spawn(::entt::registry& registry, ::entt::entity source = ::entt::null) {
        auto entity = packed_entity::default_projectile::spawn(registry, source);
        if (auto* sprite = registry.try_get<Sprite>(entity)) {
            sprite->sprite.setTexture(
                resourceloader.load<
                sf::Texture, sf::TextureLoader
                >("res/textures/e_projectile/atlas.png")
            );
        }

        return entity;
    }
}

namespace sprite_animation_entity {
    inline ::entt::entity spawn(
        ::entt::registry& registry,
        const std::string& sprite_path,
        const std::string& sheet_path,
        const std::string& anim_name = "default"
    ) {
        
        ::entt::entity entity = registry.create(); {
            auto& transform = registry.emplace<Transform>(entity);

            auto& sprite = registry.emplace<Sprite>(entity,
                resourceloader.load<sf::Texture, sf::TextureLoader>(sprite_path)
            );
            auto& sprite_animation = registry.emplace<SpriteAnimation>(entity,
                resourceloader.load<Spritesheet::Resource, Spritesheet::Loader>(sheet_path)
            ); {
                sprite_animation.play(anim_name);
                //
            }
        }
        return entity;
    }
};

namespace twotaunt {
    inline ::entt::entity spawn(::entt::registry& registry, float multiplier = 1.f) {
        auto enitity = packed_entity::sprite_animation_entity::spawn(registry,
            "res/textures/cmex/2taunt.png", "res/textures/cmex/spritesheet.json", "default"
        ); if (auto* t = registry.try_get<Transform>(enitity)) {
            t->scale = {0.35f, 0.35f};
            t->position.x = game_rng::random<float>(32.f * multiplier, 256.f * multiplier);
            t->position.y = game_rng::random<float>(15.f * multiplier, 64.f * multiplier);
        } if (auto* s = registry.try_get<Sprite>(enitity)) { s->center = false; }
        registry.emplace_or_replace<ZIndex>(enitity, 2);

        return enitity;
    }

};

namespace gad {
    inline ::entt::entity spawn(::entt::registry& registry) {
        ::entt::entity entity = registry.create(); {
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
    inline ::entt::entity spawn(::entt::registry& registry) {
        ::entt::entity entity = registry.create(); {
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
            attack.spawn_func = packed_entity::mo4a_projectile::spawn;
            registry.emplace<MobAttackRanged>(entity);
            
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

            auto& healthbar = registry.emplace<HealthBar>(entity); {
                healthbar.offset = sprite.offset + sf::Vector2f(-4.f, -6.f);
                healthbar.size = {24.f, 3.5f};
                healthbar.color = sf::Color::Black;
                healthbar.color_empty = sf::Color::Red;
                healthbar.color_full = sf::Color::Green;
            }
        }
        
        return entity;
    }
};

namespace mob_spawner {
    inline ::entt::entity spawn(::entt::registry& registry, std::function<::entt::entity(::entt::registry&)> func = nullptr) {
        auto entity = registry.create();
        registry.emplace<ZIndex>(entity, 1);
        auto& transform = registry.emplace<Transform>(entity);
        auto& mob_spawner = registry.emplace<MobSpawner>(entity); {
            mob_spawner.spawn_func = func;
            mob_spawner.spawn_interval = 5.5f;
            mob_spawner.spawn_range = sf::Vector2(-450.f, 450.f);
            mob_spawner.spawn_soundbuffer = resourceloader.load<sf::SoundBuffer, sf::SoundBufferLoader>("res/audio/wither-spawn.mp3");
        }
    
    
        registry.emplace<Sprite>(entity, resourceloader.load<sf::Texture, sf::TextureLoader>("res/textures/spawner/atlas.png"));
        auto& sprite_anim = registry.emplace<SpriteAnimation>(
            entity,
            resourceloader.load<Spritesheet::Resource, Spritesheet::Loader>("res/textures/spawner/spritesheet.json")
        ); {
            sprite_anim.play("idle");
        }
            
        return entity;
    }
}

namespace player {
    inline ::entt::entity spawn(::entt::registry& registry) {
        auto entity = registry.create();
        registry.emplace<ZIndex>(entity, 1);
        registry.emplace<Transform>(entity).position = {50.f , 50.f};
        registry.emplace<Velocity>(entity, 0.f, 0.f, true);
        registry.emplace<SpriteAnimationControl>(entity);
        registry.emplace<PlayerInput>(entity);
        
        auto& attack = registry.emplace<Attack>(entity); {
            attack.spawn_func = packed_entity::default_projectile::spawn;
            attack.interval = 0.2f; 
        }
        
        registry.emplace<Faction>(entity, "player");
        registry.emplace<Health>(entity, 100.f, 100.f);
        registry.emplace<MoveSpeed>(entity, 100.0f);
        
        Hitbox player_hitbox;
            player_hitbox.size = {16.f, 32.f};
            player_hitbox.offset = {-player_hitbox.size.x / 2.f, -player_hitbox.size.y / 2.f};
            registry.emplace<Hitbox>(entity, player_hitbox);
    
        Sprite sprite(resourceloader.load<sf::Texture, sf::TextureLoader>("res/textures/vlad/atlas.png"));
            sprite.offset = {-8.f, -16.f};
            registry.emplace<Sprite>(entity, sprite);
    
        SpriteAnimation sprite_anim;
            sprite_anim.spritesheet = resourceloader.load<Spritesheet::Resource, Spritesheet::Loader>("res/textures/vlad/spritesheet.json");
            registry.emplace<SpriteAnimation>(entity, sprite_anim);
    
        HealthBar healthbar;
            healthbar.offset = sprite.offset + sf::Vector2f(-4.f, -6.f);
            healthbar.size = {24.f, 3.5f};
            healthbar.color = sf::Color::Black;
            healthbar.color_empty = sf::Color::Red;
            healthbar.color_full = sf::Color::Green;
         
            registry.emplace<HealthBar>(entity, healthbar);
    
        Camera player_camera;
            player_camera.view = sf::View(
                    {0.f, 0.f},
                    {
                        static_cast<float>(Singleton::Variables::WINDOW_SIZE.x) / 3.0f,
                        static_cast<float>(Singleton::Variables::WINDOW_SIZE.y) / 3.0f
                    }
                );
                 
            registry.emplace<Camera>(entity, player_camera);
        
        return entity;
    }
}

}; // namespace packed_entity