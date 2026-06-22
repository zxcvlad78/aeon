#include "UIComponentSystems.hpp"

void ui_render_system(entt::registry& registry, sf::RenderWindow& window) {
    render_healthbar(registry, window);
}

void render_healthbar(entt::registry& registry, sf::RenderWindow& window) {
    auto view = registry.view<UIHealthBar, Transform, Health>();
    
    for (auto [entity, ui_healthbar, transform, health] : view.each()) {
        sf::Vector2f pos = transform.position + ui_healthbar.offset;

        sf::RectangleShape color_bar(sf::Vector2f(ui_healthbar.size.x, ui_healthbar.size.y));
        color_bar.setPosition(pos);
        color_bar.setFillColor(ui_healthbar.color);
        color_bar.setOutlineColor(ui_healthbar.outline_color);
        color_bar.setOutlineThickness(ui_healthbar.outline_thickness);
        window.draw(color_bar);
    
        float max_health = health.get_max_value();
        float current_health = health.get_value();

        float ratio = max_health > 0.0f ? (current_health / max_health) : 0.0f;
        float width = ui_healthbar.size.x * ratio;
        
        sf::RectangleShape full_bar(sf::Vector2f(width, ui_healthbar.size.y));
        full_bar.setPosition(pos);
        full_bar.setFillColor(ratio < 0.5f ? ui_healthbar.color_empty : ui_healthbar.color_full);

        window.draw(full_bar);

    }
}