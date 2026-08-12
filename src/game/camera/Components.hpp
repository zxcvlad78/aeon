#pragma once
#include "Components.hpp"
#include <SFML/Graphics.hpp>

struct Camera {
    sf::View view;
    float zoom = 1.0f;
    bool smooth = true;
    bool follow = true;
};