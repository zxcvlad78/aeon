#include "math.hpp"

namespace Math {
    float get_distance(sf::Vector2f vec_a, sf::Vector2f vec_b) {
        sf::Vector2f direction = {vec_b.x - vec_a.x, vec_b.y - vec_a.y};
        return std::sqrt(direction.x * direction.x + direction.y * direction.y);
    }

    sf::Vector2f get_direction(sf::Vector2f vec_a, sf::Vector2f vec_b) {
        sf::Vector2f direction = {vec_b.x - vec_a.x, vec_b.y - vec_a.y};
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance != 0) {
            direction.x /= distance;
            direction.y /= distance;
        }

        return direction;
    }


    sf::Color lerp_color(const sf::Color& a, const sf::Color& b, float t) {
        t = std::clamp(t, 0.0f, 1.0f);
        return sf::Color(
            static_cast<uint8_t>(a.r + (b.r - a.r) * t),
            static_cast<uint8_t>(a.g + (b.g - a.g) * t),
            static_cast<uint8_t>(a.b + (b.b - a.b) * t),
            static_cast<uint8_t>(a.a + (b.a - a.a) * t)
        );
    }
}