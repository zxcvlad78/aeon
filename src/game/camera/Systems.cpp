#include "Systems.hpp"

namespace CameraSystems {
    void update(entt::registry& registry, sf::RenderWindow& window, float dt) {
        auto view = registry.view<Transform, Camera>();

        for (auto [entity, transform, camera] : view.each()) {
            sf::Vector2f current_center = camera.view.getCenter();
            sf::Vector2f target_center = {transform.position.x, transform.position.y};
            sf::Vector2f lex = target_center;

            if (camera.smooth) {
                lex.x = current_center.x + (target_center.x - current_center.x) * 5.0f * dt;
                lex.y = current_center.y + (target_center.y - current_center.y) * 5.0f * dt;
            }


            camera.view.setCenter(lex);
            auto window_size = static_cast<sf::Vector2f>(window.getSize());
            camera.view.setSize(window_size / camera.zoom);
            if (camera.is_current()) {
                sf::Listener::setPosition({lex.x, lex.y, 0.f});
                window.setView(camera.view);
            }
        }
    }
}