#pragma once
#include "Components.hpp"
#include <SFML/Graphics.hpp>

struct Camera {
private:
    bool current = true;

public:
    sf::View view;
    float zoom = 1.0f;
    bool smooth = true;

    bool is_current() { return current; }
    void set_current(bool value) { current = value; }
    void make_current(entt::registry& reg) {
        Camera* current_cam = get_current(reg);
        if (current_cam != nullptr) {
            current_cam->set_current(false);
        }
        current = true;
    }

    static Camera* get_current(entt::registry& reg) {
        for (auto [e, c] : reg.view<Camera>().each()) {
            if (c.is_current()) {
                return &c;
            }
        }

        return nullptr;
    }
};