#pragma once

#include "SFML/Graphics.hpp"
#include <entt/entt.hpp>

struct DebugText {
private:
    bool visible = true;
    float timer = 0.f;
    int frame_count = 0;

    sf::Font& font;
    sf::Text text;

public:
    DebugText(sf::Font& f) : font(f), text(font){
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setPosition({10.f, 10.f});
    }

    void set_visibility(bool value) {
        visible = value;
    }

    bool is_visible() {
        return visible;
    }

    void set_string(const std::string& string) {
        text.setString(string);
    }

    void add_string(const std::string& string) {
        text.setString(text.getString() + string);
    }

    void update(entt::registry& registry, float dt) {
        timer += dt;
        frame_count++;

        if (timer >= 0.5f) {
            int fps = static_cast<int>(frame_count / timer);

            auto entity_count = registry.storage<entt::entity>().size();
            // for (auto entity : registry.alive()) {
            //     entity_count++;
            // }


            set_string(
                "FPS: " + std::to_string(fps) + "\n" +
                "Entities: " + std::to_string(entity_count) + "\n"
            
            );
            
            timer = 0.0f;
            frame_count = 0;
        }
    }

    void render(sf::RenderWindow& window) {
        if (is_visible()) window.draw(text);
    }
};