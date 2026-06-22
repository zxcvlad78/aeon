#include <termcolor/termcolor.hpp>
#include "ResourceLoader.hpp"
#include "systems/ComponentSystems.hpp"
#include "systems/UIComponentSystems.hpp"

const std::string GAME_VERSION = "v0.0.1";

const sf::Vector2u WINDOW_SIZE = sf::Vector2u(800, 800);

int main() {
    //std::cout << termcolor::green << "start" << termcolor::reset << std::endl;

    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE), "Aeon (" + GAME_VERSION + ")");
    window.setFramerateLimit(144);
    
    ResourceLoader resourceloader;
    
    sf::Clock clock;


    sf::Font main_font;
    if (!main_font.openFromFile("res/fonts/main_font.ttf")) {
        return -1;
    }

    float fps_timer = 0.f;
    int frame_count = 0;

    sf::Text fpsText(main_font);
    fpsText.setCharacterSize(24);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition({10.f, 10.f});

    
    entt::registry registry;

    //Player
    {
        auto player = registry.create();
        registry.emplace<Transform>(player);
        registry.emplace<Velocity>(player, 0.f, 0.f, true);
        registry.emplace<PlayerInput>(player);
        registry.emplace<SpriteAnimationControl>(player);
        
        registry.emplace<Health>(player, 100.f, 100.f);
        registry.emplace<MoveSpeed>(player, 100.0f);
        


        Sprite sprite(resourceloader.load<sf::Texture, sf::TextureLoader>("res/textures/vlad/atlas.png"));
            sprite.offset = {-8.f, -16.f};
            registry.emplace<Sprite>(player, sprite);
       
        SpriteAnimation sprite_anim;
            sprite_anim.spritesheet = resourceloader.load<Spritesheet::Resource, Spritesheet::Loader>("res/player_spritesheet.json");
            registry.emplace<SpriteAnimation>(player, sprite_anim);

        UIHealthBar healthbar;
            healthbar.offset = sprite.offset + sf::Vector2f(-4.f, -6.f);
            healthbar.size = {24.f, 3.5f};
            healthbar.color = sf::Color::Black;
            healthbar.color_empty = sf::Color::Red;
            healthbar.color_full = sf::Color::Green;
            
            registry.emplace<UIHealthBar>(player, healthbar);

        Camera player_camera;
            player_camera.view = sf::View(
                    {0.f, 0.f},
                    {static_cast<float>(WINDOW_SIZE.x) / 4.f, static_cast<float>(WINDOW_SIZE.y) / 4.f}
                );
                    
            registry.emplace<Camera>(player, player_camera);
            

            
    }
    
    //Floor
    {
        auto floor = registry.create();
            registry.emplace<Transform>(floor);
            registry.emplace<Sprite>(floor, resourceloader.load<sf::Texture, sf::TextureLoader>("res/textures/floor.png"));
    }

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            
        }

        sf::Time elapsed = clock.restart();
        float delta_time = elapsed.asSeconds();



        player_input_system(registry);
        movement_system(registry, delta_time);
        camera_system(registry, window, delta_time);
        
        // Fps counter
        {
            fps_timer += delta_time;
            frame_count++;

            if (fps_timer >= 0.5f) {
                int fps = static_cast<int>(frame_count / fps_timer);
                fpsText.setString("FPS: " + std::to_string(fps));
                
                fps_timer = 0.0f;
                frame_count = 0;
            }
        }

        window.clear(sf::Color::Black);

        sprite_animation_control_system(registry);
        sprite_animation_system(registry, delta_time);
        
        projectile_system(registry,  delta_time);
        render_system(registry, window);
        ui_render_system(registry, window);

        window.setView(window.getDefaultView()); 
        window.draw(fpsText);
        
        window.display();
    }

    std::cout << termcolor::yellow << "end" << termcolor::reset << std::endl;
    return 0;
}
