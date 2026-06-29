#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

class Console {
private:
    sf::Vector2f position = sf::Vector2f(0.f, 0.f);
    bool visible = true;
    
    sf::RectangleShape background_rect;
    sf::RectangleShape input_rect;
    sf::RectangleShape head_rect;

    std::unique_ptr<sf::Text> input_rect_text;
    std::unique_ptr<sf::Text> output_text;
    std::unique_ptr<sf::Text> head_title;

    std::string input_string = "";
    std::string total_string = "";

    Console() = default;

public:
    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;
    ~Console() = default;

    static Console& get_instance();

    void init(sf::Font& f, uint16_t character_size = 16);
    
    void set_position(sf::Vector2f p);
    sf::Vector2f get_position() { return position; }

    void switch_visibility();
    void print(const std::string& text, sf::Color color = sf::Color::White);

    void update(sf::RenderWindow& window, float dt);
    void handle_input(sf::RenderWindow& window);
    void render(sf::RenderWindow& window, float dt);
};
