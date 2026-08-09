#pragma once
#include <random>

namespace game_rng {

inline std::mt19937& rng() {
    static std::mt19937 gen(std::random_device{}());
    return gen;
}

template<typename T>
T random(T min, T max) {
    if constexpr (std::is_integral_v<T>) {
        std::uniform_int_distribution<T> dist(min, max);
        return dist(rng());
    } else if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<T> dist(min, max);
        return dist(rng());
    } else {
        static_assert(std::is_arithmetic_v<T>, "T must be arithmetic");
    }
}


};