#include "Systems.hpp"

#include "../include/Components.hpp"
#include "../sprite/Systems.hpp"

namespace RenderSystems {
    void update(entt::registry& registry, sf::RenderWindow& window) {
        if (!enabled) return;
        SpriteSystems::draw(registry, window);

        if (draw_hitboxes_enabled) draw_hitboxes(registry, window);
    }

    void draw_hitboxes(entt::registry& registry, sf::RenderWindow& window) {
        for (auto [entity, transform, hitbox] : registry.view<Transform, Hitbox>().each()) {
            sf::Vector2f pos = transform.position + hitbox.offset;
    
            sf::RectangleShape color_bar(hitbox.size);
            color_bar.setPosition(pos);
            color_bar.setFillColor(sf::Color(255, 0, 0, 126.f));
            window.draw(color_bar);
        }
    }
}