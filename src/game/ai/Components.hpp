#pragma once
#include "../../../include/Components.hpp"

namespace AIComponents {
    struct AITarget {
        float update_interval = 5.f; //sex (seconds)
        float time_elapsed = 0.f;
        
        void pick(entt::entity e) {
            entity = e;
            time_elapsed = 0.f;
        }
        
        entt::entity get_entity() { return entity; }
        bool should_pick() { return time_elapsed >= update_interval; }
        
        private:
        entt::entity entity = entt::null;
    };
};
