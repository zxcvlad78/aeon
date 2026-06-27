#include "math.hpp"

namespace Math {
    sf::Vector2f get_direction(sf::Vector2f vec_a, sf::Vector2f vec_b) {
        sf::Vector2f direction = {vec_b.x - vec_a.x, vec_b.y - vec_b.y};
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance != 0) {
            direction.x /= distance;
            direction.y /= distance;
        }

        return direction;
    }

}