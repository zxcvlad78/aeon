#pragma once
#include <Components.hpp>

struct UIHealthBar {
    sf::Vector2f size = {32.f, 8.f};
    sf::Vector2f offset = {0.f, 0.f};
    sf::Color color = sf::Color::White;
    sf::Color color_empty = sf::Color::Black;
    sf::Color color_full = sf::Color::White;
    sf::Color outline_color = sf::Color::Black;
    float outline_thickness = 1.0f;
};