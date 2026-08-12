#pragma once
#include <entt/entt.hpp>
#include "../../../include/Components.hpp"


namespace Singleton {
    namespace Variables {
        extern sf::Font main_font;
        extern bool speed_scale;
        const std::string GAME_VERSION = "v0.0.1";
        const sf::Vector2u WINDOW_SIZE = sf::Vector2u(1280, 720);
    }
}