#include <termcolor/termcolor.hpp>
#include "ResourceLoader.hpp"
#include "Systems.hpp"

void input_system(entt::registry& registry);
void movement_system(entt::registry& registry, float dt);
void camera_system(entt::registry& registry, sf::RenderWindow& window, float dt);
void render_system(entt::registry& registry, sf::RenderWindow& window);
void sprite_animation_system(entt::registry& registry, float dt);
void sprite_animation_control_system(entt::registry& registry);

const sf::Vector2u WINDOW_SIZE = sf::Vector2u(800, 800);

int main() {
    //std::cout << termcolor::green << "start" << termcolor::reset << std::endl;

    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE), "Vladot Engine");
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
            registry.emplace<Position>(player, 0.f, 0.f);
            registry.emplace<Velocity>(player, 0.f, 0.f, true);
            registry.emplace<MoveSpeed>(player, 100.0f);
            registry.emplace<MoveDirection>(player);
            registry.emplace<SpriteAnimationControl>(player);

        Sprite sprite(resourceloader.load<sf::Texture, sf::TextureLoader>("res/textures/vlad/atlas.png"));
            registry.emplace<Sprite>(player, sprite);
        
        Camera player_camera;
            player_camera.view = sf::View(
                    {0.f, 0.f},
                    {static_cast<float>(WINDOW_SIZE.x) / 4.f, static_cast<float>(WINDOW_SIZE.y) / 4.f}
                );
                    
            registry.emplace<Camera>(player, player_camera);
            

            
            SpriteAnimation sprite_anim;
                sprite_anim.spritesheet = resourceloader.load<Spritesheet::Resource, Spritesheet::Loader>("res/player_spritesheet.json");
                registry.emplace<SpriteAnimation>(player, sprite_anim);
    }
    
    //Floor
    {
        auto floor = registry.create();
            registry.emplace<Position>(floor, 0.f, 0.f);
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

        input_system(registry);
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
        render_system(registry, window);

        window.setView(window.getDefaultView()); 
        window.draw(fpsText);
        
        window.display();
    }

    std::cout << termcolor::yellow << "end" << termcolor::reset << std::endl;
    return 0;
}
