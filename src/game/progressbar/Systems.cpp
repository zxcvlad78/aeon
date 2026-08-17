#include "Systems.hpp"
#include <Aeon.hpp>

namespace ProgressBarSystems {
    inline void healthbar(entt::registry& registry) {
        auto view = registry.view<GluedTo, ProgressBar, ProgressBarType::HealthBar>();
        for (auto [e, gt, pb] : view.each()) {
            auto glued_entity = gt.entity;
            if (!registry.valid(glued_entity)) {
                registry.destroy(e);
                continue;
            }
            if (const Health* health = registry.try_get<Health>(glued_entity)) {
                pb.set_value(health->get_value());
                pb.set_max_value(health->get_max_value());
            }
        }
    }

    void update(entt::registry& registry) {
        healthbar(registry);
        auto view = registry.view<ProgressBar, Transform>();
        
        for (auto [entity, progressbar, transform] : view.each()) {
            sf::Vector2f pos = transform.position + progressbar.offset;
            
            progressbar.bg_rect.setSize(sf::Vector2f(progressbar.size.x, progressbar.size.y));
            progressbar.bg_rect.setPosition(pos);
            progressbar.bg_rect.setFillColor(progressbar.bg_color);
            progressbar.bg_rect.setOutlineColor(progressbar.outline_color);
            progressbar.bg_rect.setOutlineThickness(progressbar.outline_thickness);
    
            float current_value = progressbar.get_value();
            float max_value = progressbar.get_max_value();
            float ratio = max_value > 0.0f ? (current_value / max_value) : 0.0f;
            float width = progressbar.size.x * ratio;
            
            progressbar.fg_rect.setSize(sf::Vector2f(width, progressbar.size.y));
            progressbar.fg_rect.setPosition(pos);
            progressbar.fg_rect.setFillColor(
                Math::lerp_color(progressbar.color_empty, progressbar.color_full, ratio)
            );
        }
    }
    void render(entt::registry& registry, sf::RenderWindow& window) {
        auto view = registry.view<ProgressBar>();
        
        for (auto [entity, progressbar] : view.each()) {
            window.draw(progressbar.bg_rect);
            window.draw(progressbar.fg_rect);
        }
    }
}