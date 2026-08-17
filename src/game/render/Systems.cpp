#include "Systems.hpp"
#include <Aeon.hpp>

namespace RenderSystems {
    void update(entt::registry& registry, sf::RenderWindow& window) {
        if (!enabled) return;

        TileMapSystems::render(registry, window);
        SpriteSystems::render(registry, window);
        ProgressBarSystems::render(registry, window);

        if (render_hitboxes_enabled) render_hitboxes(registry, window);
    }

    void render_hitboxes(entt::registry& registry, sf::RenderWindow& window) {
        for (auto [entity, transform, hitbox] : registry.view<Transform, Hitbox>().each()) {
            sf::Vector2f pos = transform.position + hitbox.offset;
    
            sf::RectangleShape color_bar(hitbox.size);
            color_bar.setPosition(pos);
            color_bar.setFillColor(sf::Color(255, 0, 0, 126.f));
            window.draw(color_bar);
        }
    }
}