#include "Console.hpp"
#include "../game/singleton/Singleton.hpp"

Console& Console::get_instance() {
    static Console instance;
    return instance;
}


void Console::init(sf::Font& f, uint16_t character_size) {
    input_rect_text = std::make_unique<sf::Text>(f);
    output_text = std::make_unique<sf::Text>(f);
    head_title = std::make_unique<sf::Text>(f);

    output_text->setCharacterSize(character_size);
    output_text->setFillColor(sf::Color::White);

    head_title->setCharacterSize(24);
    head_title->setFillColor(sf::Color::White);
    head_title->setString("Console");
    head_title->setPosition({5.f, 0.f});

    input_rect_text->setCharacterSize(character_size);
    input_rect_text->setFillColor(sf::Color::White);

    background_rect.setFillColor(sf::Color(0, 0, 0, 232)); 
    background_rect.setSize({
        static_cast<float>(Singleton::Variables::WINDOW_SIZE.x),
        static_cast<float>(Singleton::Variables::WINDOW_SIZE.y)
    });

    input_rect.setFillColor(sf::Color(10, 10, 10, 255));
    input_rect.setSize({
        static_cast<float>(Singleton::Variables::WINDOW_SIZE.x),
        static_cast<float>(Singleton::Variables::WINDOW_SIZE.y) / 10.f
    });
    input_rect.setPosition({
        0.f,
        static_cast<float>(Singleton::Variables::WINDOW_SIZE.y) - input_rect.getSize().y
    });
    input_rect_text->setPosition(input_rect.getPosition());

    head_rect.setFillColor(sf::Color(20, 20, 20, 255));
    head_rect.setSize({
        static_cast<float>(Singleton::Variables::WINDOW_SIZE.x),
        static_cast<float>(head_title->getCharacterSize() + head_title->getCharacterSize() / 2.f)
    });

    output_text->setPosition({
        5.f,
        head_rect.getSize().y
    });

    Console::print("Console initialized!");
}

void Console::set_position(sf::Vector2f p) {
    position = p;
}

void Console::print(const std::string& text, sf::Color color) {
    total_string += (text + "\n");
    output_text->setString(total_string);
}

void Console::render(sf::RenderWindow& window, float dt) {
    if (!visible) return;

    window.draw(background_rect);
    window.draw(input_rect);
    window.draw(*input_rect_text);
    window.draw(*output_text);
    window.draw(head_rect);
    window.draw(*head_title);
}

void Console::handle_input(sf::RenderWindow& window) {
    
}


void Console::update(sf::RenderWindow& window, float dt) {
    if (!visible) return;

    std::string t = input_string;
    if (t.empty()) {
        input_rect_text->setFillColor(sf::Color(127, 127, 127));
        t = "Enter the command";
    } else { input_rect_text->setFillColor(sf::Color::White); }

    input_rect_text->setString(t);

    handle_input(window);
    render(window, dt);
}

void Console::switch_visibility() {
    visible = !visible;
}