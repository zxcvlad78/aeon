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
            sf::Listener::setPosition({lex.x, lex.y, 0.f});
        }

        auto camera_view = registry.view<Camera>();
        if (!camera_view.empty()) {
            auto camera_entity = camera_view.front();
            auto& cam = camera_view.get<Camera>(camera_entity);
            window.setView(cam.view);
        }
    }
}