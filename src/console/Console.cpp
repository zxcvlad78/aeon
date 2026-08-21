#include "Console.hpp"
#include "../game/singleton/Singleton.hpp"
#include <algorithm>
#include <sstream>
#include <cmath>
#include <string_view>


Console& Console::get_instance() {
    static Console instance;
    return instance;
}

inline bool is_word_char(char ch) {
    return std::isalnum(static_cast<unsigned char>(ch));
}

inline int prev_word_position(int pos, const std::string& str) {
    if (pos <= 0) return 0;
    int i = pos - 1;
    while (i >= 0 && !is_word_char(str[i])) --i;
    while (i >= 0 && is_word_char(str[i])) --i;
    return i + 1;
}
inline int next_word_position(int pos, const std::string& str) {
    int len = str.size();
    if (pos >= len) return len;
    int i = pos;
    while (i < len && !is_word_char(str[i])) ++i;
    while (i < len && is_word_char(str[i])) ++i;
    return i;
}

void Console::init(sf::Font& f, uint16_t character_size) {
    font_ptr = &f;
    char_size = character_size;
    
    input_rect_text = std::make_unique<sf::Text>(f);
    output_text = std::make_unique<sf::Text>(f);
    head_title = std::make_unique<sf::Text>(f);
    suggestion_text = std::make_unique<sf::Text>(f);
    
    background_rect.setFillColor(background_color);
    background_rect.setOutlineThickness(theme_outline_thickness);
    background_rect.setOutlineColor(theme_outline_color);
    
    head_rect.setFillColor(foreground_color);
    head_rect.setOutlineThickness(theme_outline_thickness);
    head_rect.setOutlineColor(theme_outline_color);
    
    head_title->setCharacterSize(24);
    head_title->setFillColor(sf::Color(180, 180, 200));
    head_title->setString("Console");
    
    input_rect.setFillColor(foreground_color);
    input_rect.setOutlineThickness(theme_outline_thickness);
    input_rect.setOutlineColor(theme_outline_color);
    
    input_rect_text->setCharacterSize(char_size);
    input_rect_text->setFillColor(sf::Color(220, 220, 220));
    
    output_text->setCharacterSize(char_size);
    output_text->setFillColor(sf::Color::White);
    
    suggestion_rect.setFillColor(sf::Color::Blue);


    suggestion_text->setCharacterSize(char_size - 2);
    suggestion_text->setFillColor(sf::Color(150, 150, 150, 200));
    
    scrollbar_track.setFillColor(sf::Color(40, 40, 45, 150));
    scrollbar_thumb.setFillColor(sf::Color(100, 100, 110, 200));
    
    cursor_rect.setFillColor(sf::Color(255, 255, 255, 200));
    cursor_rect.setSize({2.f, static_cast<float>(char_size)});
    
    register_default_commands();
    print_success("Console initialized! Type 'help' for available commands");
}

void Console::show() { visible = true; }
void Console::hide() { visible = false; }
void Console::toggle() { visible = !visible; }

sf::Color parse_color(const std::vector<std::string>& args) {
    if (args.empty()) { return sf::Color(0, 0, 0, 255); }

    std::uint8_t r = 0, g = 0, b = 0, a = 255;
    
    try {
        if (args.size() > 0) r = static_cast<std::uint8_t>(std::stoi(args[0]));
        if (args.size() > 1) g = static_cast<std::uint8_t>(std::stoi(args[1]));
        if (args.size() > 2) b = static_cast<std::uint8_t>(std::stoi(args[2]));
        if (args.size() > 3) a = static_cast<std::uint8_t>(std::stoi(args[3]));
    } catch (const std::exception& e) {
        return sf::Color(0, 0, 0, 255);
    }
    
    return sf::Color(r, g, b, a);
}

void Console::print(const std::string& text, sf::Color color) {
    messages.push_back({text, color});
    if (messages.size() > max_messages) messages.pop_front();
    
    total_string.clear();
    for (const auto& msg : messages) {
        total_string += msg.text + "\n";
    }
    output_text->setString(total_string);
}

void Console::print_error(const std::string& text) {
    print("[ERROR] " + text, sf::Color(255, 80, 80));
}

void Console::print_warning(const std::string& text) {
    print("[WARNING] " + text, sf::Color(255, 200, 50));
}

void Console::print_success(const std::string& text) {
    print("[OK] " + text, sf::Color(100, 255, 100));
}

void Console::register_command(const std::string& name,
                         std::function<void(const std::vector<std::string>&)> handler,
                         const std::string& description,
                         const std::string& usage) {
    commands[name] = { name, handler, description, usage };
}

void Console::unregister_command(const std::string& name) {
    commands.erase(name);
}

void Console::execute_command(const std::string& command_line) {
    if (command_line.empty()) return;
    
    add_to_history(command_line);
    print("> " + command_line, sf::Color(150, 150, 150));
    
    std::istringstream iss(command_line);
    std::string cmd_name;
    iss >> cmd_name;
    
    auto it = commands.find(cmd_name);
    if (it != commands.end()) {
        std::vector<std::string> args;
        std::string arg;
        while (iss >> arg) args.push_back(arg);
        
        try {
            it->second.handler(args);
        } catch (const std::exception& e) {
            print_error("Command execution failed: " + std::string(e.what()));
        }
    } else {
        print_error("Unknown command: " + cmd_name);
    }
}

std::vector<std::string> Console::get_suggestions(const std::string& prefix) {
    std::vector<std::string> suggestions;
    for (const auto& [name, cmd] : commands) {
        if (name.find(prefix) == 0) suggestions.push_back(name);
    }
    std::sort(suggestions.begin(), suggestions.end());
    return suggestions;
}

void Console::autocomplete() {
    if (input_string.empty()) return;
    
    auto suggestions = get_suggestions(input_string);

    if (suggestions.size() == 1) {
        input_string = suggestions[0] + " ";
    } else if (suggestions.size() > 1) {
        std::string common_prefix = suggestions[0];
        for (size_t i = 1; i < suggestions.size(); ++i) {
            size_t j = 0;
            while (j < common_prefix.length() && j < suggestions[i].length() && 
                   common_prefix[j] == suggestions[i][j]) {
                j++;
            }
            common_prefix = common_prefix.substr(0, j);
        }
        
        if (common_prefix.length() > input_string.length()) {
            input_string = common_prefix;
        }
    }

    cursor_position = input_string.length();
}

void Console::handle_event(const sf::Event& event, sf::RenderWindow& window) {
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* key = event.getIf<sf::Event::KeyPressed>();
        if (!key) return;
        
        if (key->code == sf::Keyboard::Key::Grave) {
            toggle();
            return;
        }

        if (!visible) return;

        bool is_ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl);
        
        cursor_blink_time = 0.f;
        cursor_visible = true;

        switch (key->code) {
            case sf::Keyboard::Key::Escape:
                hide();
                break;
            case sf::Keyboard::Key::Enter:
                execute_command(input_string);
                input_string.clear();
                cursor_position = 0;
                break;
            case sf::Keyboard::Key::Backspace:
                if (cursor_position > 0 && !input_string.empty()) {
                    if (is_ctrl) {
                        int start = prev_word_position(cursor_position, input_string);
                        if (start < cursor_position) {
                            input_string.erase(start, cursor_position - start);
                            cursor_position = start;
                        }
                    } else {
                        input_string.erase(cursor_position - 1, 1);
                        cursor_position--;
                    }
                }
                break;
            case sf::Keyboard::Key::Delete:
                if (cursor_position < input_string.length() && !input_string.empty()) {
                    input_string.erase(cursor_position, 1);
                }
                break;
            case sf::Keyboard::Key::Left:
                if (is_ctrl) {
                    cursor_position = prev_word_position(cursor_position, input_string);
                }
                else if (cursor_position > 0) cursor_position--;
                break;
            case sf::Keyboard::Key::Right:
                if (is_ctrl) {
                    cursor_position = next_word_position(cursor_position, input_string);
                }
                else if (cursor_position < input_string.length()) cursor_position++;
                break;
            case sf::Keyboard::Key::Home:
                cursor_position = 0;
                break;
            case sf::Keyboard::Key::End:
                cursor_position = input_string.length();
                break;
            case sf::Keyboard::Key::Up:
                if (!command_history.empty() && history_index < static_cast<int>(command_history.size()) - 1) {
                    history_index++;
                    input_string = command_history[command_history.size() - 1 - history_index];
                    cursor_position = input_string.length();
                }
                break;
            case sf::Keyboard::Key::Down:
                if (history_index > 0) {
                    history_index--;
                    input_string = command_history[command_history.size() - 1 - history_index];
                    cursor_position = input_string.length();
                } else if (history_index == 0) {
                    history_index = -1;
                    input_string.clear();
                    cursor_position = 0;
                }
                break;
            case sf::Keyboard::Key::Tab:
                autocomplete();
                break;
            case sf::Keyboard::Key::PageUp:
                scroll_offset -= scroll_speed * 10;
                clamp_scroll();
                break;
            case sf::Keyboard::Key::PageDown:
                scroll_offset += scroll_speed * 10;
                clamp_scroll();
                break;
            default:
                break;
        }
    }
    
    if (!visible) return;

    if (event.is<sf::Event::TextEntered>()) {
        const auto* text = event.getIf<sf::Event::TextEntered>();
        if (text && text->unicode >= 32 && text->unicode < 127) {
            input_string.insert(cursor_position, 1, static_cast<char>(text->unicode));
            cursor_position++;
        }
    }
    
    if (event.is<sf::Event::MouseWheelScrolled>()) {
        const auto* scroll = event.getIf<sf::Event::MouseWheelScrolled>();
        if (scroll) {
            scroll_offset -= scroll->delta * scroll_speed;
            clamp_scroll();
        }
    }
}

void Console::update(sf::RenderWindow& window, float dt) {
    if (!visible) return;
    
    update_cursor(dt);
    
    const auto window_size = static_cast<sf::Vector2f>(window.getSize());
    
    background_rect.setFillColor(background_color);
    background_rect.setOutlineThickness(theme_outline_thickness);
    background_rect.setOutlineColor(theme_outline_color);

    background_rect.setSize({window_size.x, console_height});
    background_rect.setPosition({0.f, 0.f});
    
    suggestion_rect.setFillColor(foreground_color);

    head_rect.setFillColor(foreground_color);
    head_rect.setOutlineThickness(theme_outline_thickness);
    head_rect.setOutlineColor(theme_outline_color);

    head_rect.setSize({window_size.x, 35.f});
    head_rect.setPosition({0.f, 0.f});
    
    head_title->setPosition({10.f, 5.f});

    input_rect.setFillColor(foreground_color);
    input_rect.setOutlineThickness(theme_outline_thickness);
    input_rect.setOutlineColor(theme_outline_color);

    input_rect_text->setString(input_string.empty() ? "Enter command..." : input_string);
    input_rect_text->setFillColor(input_string.empty() ? 
        sf::Color(100, 100, 100, 150) : 
        sf::Color(220, 220, 220));
    
    float text_height = input_rect_text->getLocalBounds().size.y;
    float input_height = text_height + input_padding * 2;
    input_height = std::max(input_height, 30.f);
    
    input_rect.setSize({window_size.x - 20.f, input_height});
    input_rect.setPosition({10.f, console_height - input_height - 10.f});
    
    input_rect_text->setCharacterSize(char_size);
    input_rect_text->setPosition({
        input_rect.getPosition().x + input_padding / 2.f,
        input_rect.getPosition().y + input_padding
    });
    
    cursor_rect.setSize({2.f, input_rect.getSize().y - 5.f});
    float cursor_x = input_rect_text->findCharacterPos(cursor_position).x;
    float cursor_y = input_rect.getPosition().y + (input_rect.getSize().y - cursor_rect.getSize().y) / 2.f;
    cursor_rect.setPosition({cursor_x, cursor_y});
    
    float output_y = head_rect.getSize().y + 5.f;
    output_text->setPosition({15.f, output_y - scroll_offset});
    output_text->setCharacterSize(char_size);
    
    previous_max_scroll = max_scroll;
    was_at_bottom = (scroll_offset >= max_scroll - 2.f);

    update_scrollbar(window);
    clamp_scroll();

    if (was_at_bottom && messages.size() > 0) {
        scroll_offset = max_scroll;
        clamp_scroll();
    }

    suggestion_text->setString("");
    suggestion_text->setCharacterSize(char_size);
    if (!input_string.empty()) {
        auto suggestions = get_suggestions(input_string);
        if (!suggestions.empty()) {
            std::string combined;

            if (suggestions.size() == 1) combined = suggestions[0];
            else {
                for (auto s : suggestions) combined += s + "\n";
            }

            suggestion_text->setString(combined);
            sf::Vector2f suggestion_text_size = suggestion_text->getLocalBounds().size;
            suggestion_text->setPosition({
                input_rect_text->getPosition().x,
                input_rect.getPosition().y - suggestion_text_size.y - char_size
            });

            static float suggestion_rect_offset = 5.f; 
            suggestion_rect.setSize({
                suggestion_text_size.x + suggestion_rect_offset,
                suggestion_text_size.y + suggestion_rect_offset,
            });

            sf::Vector2f suggestion_text_pos = suggestion_text->getPosition();
            suggestion_rect.setPosition({
                suggestion_text_pos.x - (suggestion_rect_offset / 2.f),
                suggestion_text_pos.y - (suggestion_rect_offset / 2.f)
            });
        }
    }
    //
}

void Console::render(sf::RenderWindow& window) {
    if (!visible) return;
    
    window.draw(background_rect);
    window.draw(head_rect);
    window.draw(*head_title);
    window.draw(input_rect);
    window.draw(*input_rect_text);
    
    if (cursor_visible) {
        window.draw(cursor_rect);
    }
    
    sf::View original_view = window.getView();
    
    sf::Vector2f window_size = static_cast<sf::Vector2f>(window.getSize());
    
    float view_top = head_rect.getSize().y + 5.f;
    float view_height = input_rect.getPosition().y - head_rect.getSize().y - 15.f;
    
    if (view_height > 0.f) {
        sf::View console_view(sf::FloatRect(
            sf::Vector2f(0.f, view_top),
            sf::Vector2f(window_size.x, view_height)
        ));
        
        console_view.setViewport(sf::FloatRect(
            sf::Vector2f(0.f, view_top / window_size.y),
            sf::Vector2f(1.f, view_height / window_size.y)
        ));
        
        window.setView(console_view);
        window.draw(*output_text);
    }
    
    if (suggestion_text && !suggestion_text->getString().isEmpty()) {
        window.draw(suggestion_rect);
        window.draw(*suggestion_text);
    }

    window.setView(original_view);
    
    if (max_scroll > 0.f) {
        window.draw(scrollbar_track);
        window.draw(scrollbar_thumb);
    }
}

void Console::register_default_commands() {
    register_command(
        "help",
        [this](const std::vector<std::string>& args) {
            if (args.empty()) {
                print("=== Available Commands ===", sf::Color::Cyan);
                for (const auto& [name, cmd] : commands) {
                    print("  " + name + " - " + cmd.description);
                }
            } else {
                auto it = commands.find(args[0]);
                if (it != commands.end()) {
                    print("Command: " + it->second.name);
                    print("Description: " + it->second.description);
                    print("Usage: " + it->second.usage);
                } else {
                    print_error("Unknown command: " + args[0]);
                }
            }
        },
        "Get help",
        "help <command>"
    );
    

    register_command(
        "clear",
        [this](const std::vector<std::string>& args) {
            messages.clear();
            total_string.clear();
            output_text->setString("");
        },
        "Clear console messages",
        "clear"
    );

    register_command(
        "console.char_size",
        [this](const std::vector<std::string>& args) {
            if (!args.empty()) {
                try {
                    int cs = std::stoi(args[0]);
                    char_size = cs;
                } catch (const std::exception& e) {
                    Console::get_instance().print_error(e.what());
                }
            }
        },
        "Set console font character size",
        "console.char_size <value>"
    );

    register_command(
        "console.theme_outline_thickness",
        [this](const std::vector<std::string>& args) {
            if (!args.empty()) {
                try {
                    float tot = std::stoi(args[0]);
                    theme_outline_thickness = tot;
                } catch (const std::exception& e) {
                    Console::get_instance().print_error(e.what());
                }
            }
        },
        "Set console theme outline thickness",
        "console.theme_outline_thickness <value>"
    );
    
    register_command(
        "console.theme_outline_color",
        [this](const std::vector<std::string>& args) {
            if (!args.empty()) {
                try {
                    theme_outline_color = parse_color(args);
                } catch (const std::exception& e) {
                    Console::get_instance().print_error(e.what());
                }
            }
        },
        "Set console theme outline color",
        "console.theme_outline_color <r g b a>"
    );

    register_command(
        "console.foreground_color",
        [this](const std::vector<std::string>& args) {
            if (!args.empty()) {
                try {
                    foreground_color = parse_color(args);
                } catch (const std::exception& e) {
                    Console::get_instance().print_error(e.what());
                }
            }
        },
        "Set console foreground color",
        "console.foreground_color <r g b a>"
    );
    
    register_command(
        "console.background_color",
        [this](const std::vector<std::string>& args) {
            if (!args.empty()) {
                try {
                    background_color = parse_color(args);
                } catch (const std::exception& e) {
                    Console::get_instance().print_error(e.what());
                }
            }
        },
        "Set console background color",
        "console.background_color <r g b a>"
    );

    register_command(
        "console.height",
        [this](const std::vector<std::string>& args) {
            if (!args.empty()) {
                try {
                    int ch = std::stoi(args[0]);
                    console_height = ch;
                } catch (const std::exception& e) {
                    Console::get_instance().print_error(e.what());
                    }
            }
        },
        "Set console height",
        "console.height <value>"
    );

}

void Console::add_to_history(const std::string& command) {
    if (!command.empty()) {
        command_history.push_back(command);
        if (command_history.size() > max_history) command_history.pop_front();
    }
    history_index = -1;
}

void Console::update_scrollbar(sf::RenderWindow& window) {
    float total_text_height = output_text->getLocalBounds().size.y;
    float viewport_height = input_rect.getPosition().y - head_rect.getSize().y - 15.f;
    sf::Vector2f window_size = static_cast<sf::Vector2f>(window.getSize());

    if (viewport_height <= 0.f) {
        max_scroll = 0.f;
        return;
    }
    
    max_scroll = std::max(0.f, total_text_height - viewport_height);
    clamp_scroll();
    
    if (max_scroll > 0.f) {
        float track_height = viewport_height;
        float thumb_height = std::max(20.f, (viewport_height / (total_text_height + 1.f)) * track_height);
        float thumb_pos = (scroll_offset / max_scroll) * (track_height - thumb_height);
        
        scrollbar_track.setSize({8.f, track_height});
        scrollbar_track.setPosition({
            window_size.x - 18.f,
            head_rect.getSize().y + 5.f
        });
        
        scrollbar_thumb.setSize({8.f, thumb_height});
        scrollbar_thumb.setPosition({
            window_size.x - 18.f,
            scrollbar_track.getPosition().y + thumb_pos
        });
    }
}

void Console::update_cursor(float dt) {
    cursor_blink_time += dt;
    if (cursor_blink_time >= 0.5f) {
        cursor_blink_time = 0.f;
        cursor_visible = !cursor_visible;
    }
}

void Console::clamp_scroll() {
    scroll_offset = std::max(0.f, std::min(scroll_offset, max_scroll));
}