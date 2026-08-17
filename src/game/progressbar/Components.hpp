#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

struct ProgressBar {
    sf::Vector2f size = {32.f, 8.f};
    sf::Vector2f offset = {0.f, 0.f};
    sf::Color bg_color = sf::Color::Black;
    sf::Color outline_color = sf::Color::Black;
    sf::Color color_empty = sf::Color::Black;
    sf::Color color_full = sf::Color::White;
    float outline_thickness = 1.0f;

    sf::RectangleShape bg_rect;
    sf::RectangleShape fg_rect;

    void set_value(float new_value) {
        value = std::clamp(new_value, std::numeric_limits<float>::lowest(), max_value);
    }

    void set_max_value(float new_max_value) {
        max_value = new_max_value;
        if (value > max_value) value = max_value;
    }

    float get_value() const { return value; }
    float get_max_value() const { return max_value; }

private:
    float value;
    float max_value;
};

