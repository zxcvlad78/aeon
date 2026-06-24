#include <termcolor/termcolor.hpp>
#include "ResourceLoader.hpp"
#include "SoundPlayer.hpp"
#include "systems/ComponentSystems.hpp"
#include "systems/UIComponentSystems.hpp"

#include "utils/DebugText.hpp"

const std::string GAME_VERSION = "v0.0.1";

const sf::Vector2u WINDOW_SIZE = sf::Vector2u(800, 800);

entt::entity spawn_projectile(entt::registry& registry) {
    auto projectile_entity = registry.create();
    registry.emplace<ZIndex>(projectile_entity, 2);
    
    Projectile projectile;
        projectile.hit_soundbuffer = resourceloader.load<sf::SoundBuffer, sf::SoundBufferLoader>("res/audio/bulk.wav");
        registry.emplace<Projectile>(projectile_entity, projectile);

    Transform projectile_transform;
        projectile_transform.position = {100.f, 100.f};
        registry.emplace<Transform>(projectile_entity, projectile_transform);
    
    Velocity projectile_velocity;
        projectile_velocity = {-120.f, -120.f};
        registry.emplace<Velocity>(projectile_entity, projectile_velocity);

    Hitbox projectile_hitbox;
        projectile_hitbox.size = {8.f, 8.f};
        projectile_hitbox.offset = {-projectile_hitbox.size.x / 2.f, -projectile_hitbox.size.y / 2.f};
        
        registry.emplace<Hitbox>(projectile_entity, projectile_hitbox);
    Sprite projectile_sprite(resourceloader.load<sf::Texture, sf::TextureLoader>("res/textures/t_projectile/atlas.png"));
        projectile_sprite.center = true;
        registry.emplace<Sprite>(projectile_entity, projectile_sprite);
    SpriteAnimation projectile_sprite_anim;
        projectile_sprite_anim.spritesheet = resourceloader.load<Spritesheet::Resource, Spritesheet::Loader>("res/t_projectile_spritesheet.json");
        projectile_sprite_anim.play("idle");
        registry.emplace<SpriteAnimation>(projectile_entity, projectile_sprite_anim);
    
    return projectile_entity;
}

bool debug_hitboxes = false;

int main() {
    //std::cout << termcolor::green << "start" << termcolor::reset << std::endl;

    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE), "Aeon (" + GAME_VERSION + ")");
    window.setFramerateLimit(144);
    
    sf::Clock clock;

    sf::Font main_font;
    if (!main_font.openFromFile("res/fonts/main_font.ttf")) {
        return -1;
    }

    DebugText debug_text(main_font);

    
    entt::registry registry;

    //Player
    {
        auto player = registry.create();
        registry.emplace<ZIndex>(player, 1);
        registry.emplace<Transform>(player);
        registry.emplace<Velocity>(player, 0.f, 0.f, true);
        registry.emplace<PlayerInput>(player);
        registry.emplace<SpriteAnimationControl>(player);
        
        registry.emplace<Health>(player, 100.f, 100.f);
        registry.emplace<MoveSpeed>(player, 100.0f);
        
        Hitbox player_hitbox;
            player_hitbox.size = {16.f, 32.f};
            player_hitbox.offset = {-player_hitbox.size.x / 2.f, -player_hitbox.size.y / 2.f};
            registry.emplace<Hitbox>(player, player_hitbox);

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
    
    spawn_projectile(registry);

    //Floor
    {
        auto floor = registry.create();
        registry.emplace<Transform>(floor);
        registry.emplace<Sprite>(floor, resourceloader.load<sf::Texture, sf::TextureLoader>("res/textures/floor.png"));
    }

    //MainLoop
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                sf::Keyboard::Key code = keyPressed->code;

                if (code == sf::Keyboard::Key::F3) {
                    debug_text.set_visibility(!debug_text.is_visible());
                }

                if (code == sf::Keyboard::Key::F6) {
                    debug_hitboxes = !debug_hitboxes;
                }

                if (code == sf::Keyboard::Key::Space) {
                    spawn_projectile(registry);
                }
            }
            
        }

        sf::Time elapsed = clock.restart();
        float delta_time = elapsed.asSeconds();


        player_input_system(registry);
        movement_system(registry, delta_time);
        camera_system(registry, window, delta_time);

        window.clear(sf::Color::Blue);

        sprite_animation_control_system(registry);
        sprite_animation_system(registry, delta_time);
        
        projectile_system(registry, delta_time);
        render_system(registry, window);
        ui_render_system(registry, window);

        window.setView(window.getDefaultView()); 
        
        debug_text.update(delta_time);
        debug_text.render(window);

        window.display();
    }

    std::cout << termcolor::yellow << "end" << termcolor::reset << std::endl;
    return 0;
}

