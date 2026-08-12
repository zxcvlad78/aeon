#include "Systems.hpp"
#include "../include/Components.hpp"
#include "../render/Components.hpp"

namespace SpriteSystems {
    void update(entt::registry& registry, sf::RenderWindow& window, float dt) {
        update_animation(registry, dt);
        draw(registry, window);
    }

    void draw(entt::registry& registry, sf::RenderWindow& window) {
        struct Renderable {
            sf::Sprite* sprite;
            sf::Vector2f position;
            sf::Vector2f offset;
            sf::Angle rotation;
            sf::Vector2f scale;
            int z_index;
        };

        static std::vector<Renderable> renderables;
        renderables.clear();

        for (auto [entity, transform, sprite] : registry.view<Transform, Sprite>().each()) {
            if (sprite.center) {
                auto tex_rect = sprite.sprite.getTextureRect();
                sprite.offset = {
                    -static_cast<float>(tex_rect.size.x) / 2.f,
                    -static_cast<float>(tex_rect.size.y) / 2.f
                };
            }

            int z_index = 0;
            
            if (registry.all_of<ZIndex>(entity)) {
                z_index = registry.get<ZIndex>(entity).value;
            }

            Renderable renderable;
            renderable.sprite = &sprite.sprite;
            renderable.position = transform.position;
            renderable.offset = sprite.offset;
            renderable.rotation = transform.rotation_degrees;
            renderable.scale = transform.scale;
            renderable.z_index = z_index;

            renderables.push_back(renderable);
        }

        std::sort(renderables.begin(), renderables.end(), 
            [](const Renderable& a, const Renderable& b) {
                return a.z_index < b.z_index;
            });

        for (const auto& renderable : renderables) {
            sf::Vector2f pos = renderable.position + renderable.offset;
            renderable.sprite->setPosition({pos.x, pos.y});
            renderable.sprite->setRotation({renderable.rotation});
            renderable.sprite->setScale({renderable.scale.x, renderable.scale.y});
            
            window.draw(*renderable.sprite);
        }
    }

    void update_animation(entt::registry& registry, float dt) {
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
                        if (sprite_anim.next_anim.empty()) {
                            sprite_anim.is_playing = false;
                        } else sprite_anim.play(sprite_anim.next_anim);

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

}