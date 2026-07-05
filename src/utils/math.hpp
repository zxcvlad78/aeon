#pragma once
#include <cmath>
#include "sfml/Graphics.hpp"

namespace Math {
    float get_distance(sf::Vector2f vec_a, sf::Vector2f vec_b);
    sf::Vector2f get_direction(sf::Vector2f vec_a, sf::Vector2f vec_b);

}