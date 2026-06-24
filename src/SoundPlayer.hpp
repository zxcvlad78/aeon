#pragma once
#include "ResourceLoader.hpp"

class SoundPlayer {
private:
    static constexpr size_t POOL_SIZE = 8;
    sf::SoundBuffer dummyBuffer;
    std::vector<sf::Sound> pool;
    size_t next = 0;

public:
    SoundPlayer() {
        pool.reserve(POOL_SIZE);
        for (size_t i = 0; i < POOL_SIZE; ++i) {
            pool.emplace_back(dummyBuffer);
        }
    }

    void play(sf::SoundBuffer& buffer, sf::Vector2f position = {0.f, 0.f}) {
        pool[next].stop();
        pool[next].setBuffer(buffer);
        pool[next].setPosition({position.x, position.y, 0.f});
        pool[next].setRelativeToListener(false);
        pool[next].play();
        next = (next + 1) % POOL_SIZE;
    }

    void play(entt::resource<sf::SoundBuffer> buffer, sf::Vector2f position = {0.f, 0.f}) {
        if (buffer) play(*buffer, position);
    }
};

extern SoundPlayer soundplayer;