#include <termcolor/termcolor.hpp>
#include "ResourceLoader.hpp"
#include "SoundPlayer.hpp"
#include "game/singleton/singleton.hpp"

#include "game/packed_entity/general.h"

#include "systems/ComponentSystems.hpp"
#include "systems/UIComponentSystems.hpp"

#include "game/mob/Systems.hpp"
#include "game/ai/Systems.hpp"

#include "game/mob_spawner/Components.hpp"
#include "game/mob_spawner/Systems.hpp"

#include "utils/DebugText.hpp"
#include "utils/math.hpp"
#include <ctime>

#include "console/Console.hpp"

bool debug_hitboxes = false;
bool collision_enabled = true;
bool enable_render_system = true;

float speed_scale = 1.0f;

int main() {
    std::time_t t = std::time(nullptr);
    std::tm* ltm = std::localtime(&t);
    std::cout << termcolor::green << "Game Started " << termcolor::bright_white << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << termcolor::reset << std::endl;

    sf::RenderWindow window(sf::VideoMode(Singleton::Variables::WINDOW_SIZE), "Aeon (" + Singleton::Variables::GAME_VERSION + ")");
    window.setFramerateLimit(144);
    
    sf::Clock clock;


    if (!Singleton::Variables::main_font.openFromFile("res/fonts/main_font.ttf")) {
        return -1;
    }

    Console::get_instance().init(Singleton::Variables::main_font, 16);
    Console::get_instance().register_command(
        "fps_max",
        [&window](const std::vector<std::string>& args) {
            if (!args.empty()) {
                try {
                    int fps = std::stoi(args[0]);
                    window.setFramerateLimit(fps);
                } catch (const std::exception& e) {
                    Console::get_instance().print_error(e.what());
                }
            }
        },
        "Set target framerate",
        "fps_max <value>"
    );
    Console::get_instance().register_command(
        "collision.enabled",
        [&window](const std::vector<std::string>& args) {
            if (!args.empty()) {
                try {
                    int val = std::stoi(args[0]);
                    collision_enabled = val > 0;
                    Console::get_instance().print_success("Collision enabled: " + collision_enabled);
                } catch (const std::exception& e) {
                    Console::get_instance().print_error(e.what());
                }
            }
        },
        "Set collision enabled",
        "collision.enabled <value>"
    );
    Console::get_instance().register_command(
        "render.enabled",
        [&window](const std::vector<std::string>& args) {
            if (!args.empty()) {
                try {
                    int val = std::stoi(args[0]);
                    enable_render_system = val > 0;
                    Console::get_instance().print_success("Collision enabled: " + collision_enabled);
                } catch (const std::exception& e) {
                    Console::get_instance().print_error(e.what());
                }
            }
        },
        "Set render enabled",
        "render.enabled <value>"
    );
    Console::get_instance().register_command(
        "render.enabled",
        [&window](const std::vector<std::string>& args) {
            if (!args.empty()) {
                try {
                    int val = std::stoi(args[0]);
                    enable_render_system = val > 0;
                    Console::get_instance().print_success("Render enabled: " + std::to_string(enable_render_system));
                } catch (const std::exception& e) {
                    Console::get_instance().print_error(e.what());
                }
            }
        },
        "Set render enabled",
        "render.enabled <value>"
    );
    Console::get_instance().register_command(
        "speed",
        [](const std::vector<std::string>& args) {
            if (!args.empty()) {
                try {
                    float val = std::stof(args[0]);
                    if (val >= 0.f) {
                        speed_scale = val;
                        Console::get_instance().print_success("Speed scale set to: " + std::to_string(speed_scale));
                    } else {
                        Console::get_instance().print_error("Speed scale must be positive");
                    }
                } catch (const std::exception& e) {
                    Console::get_instance().print_error(e.what());
                }
            } else {
                Console::get_instance().print_success("Current speed scale: " + std::to_string(speed_scale));
            }
        },
        "Set time speed multiplier",
        "speed <value>"
    );



    DebugText debug_text(Singleton::Variables::main_font);
    
    entt::registry registry;

    Console::get_instance().register_command(
        "spawn_enemy",
        [&registry](const std::vector<std::string>& args) {
            if (!args.empty()) {
                try {
                    auto e = packed_entity::zobi::spawn(registry);
                    auto t = registry.try_get<Transform>(e);
                    if (t) {
                        t->position.x = static_cast<float>(std::stoi(args[0]));
                        t->position.y = static_cast<float>(std::stoi(args[1]));
                    }

                } catch (const std::exception& e) {
                    Console::get_instance().print_error(e.what());
                }
            }
        },
        "Spawn",
        "spawn_enemy <x, y>"
    );

    //Player
    auto player = registry.create();
    {
        registry.emplace<ZIndex>(player, 1);
        registry.emplace<Transform>(player).position = {50.f , 50.f};
        registry.emplace<Velocity>(player, 0.f, 0.f, true);
        registry.emplace<SpriteAnimationControl>(player);
        registry.emplace<PlayerInput>(player);
        
        auto& attack = registry.emplace<Attack>(player);
        attack.spawn_func = packed_entity::default_projectile::spawn;
        
        registry.emplace<Faction>(player, "player");
        //registry.emplace<Health>(player, 100.f, 100.f);
        registry.emplace<MoveSpeed>(player, 100.0f);
        
        Hitbox player_hitbox;
            player_hitbox.size = {16.f, 32.f};
            player_hitbox.offset = {-player_hitbox.size.x / 2.f, -player_hitbox.size.y / 2.f};
            registry.emplace<Hitbox>(player, player_hitbox);
    
        Sprite sprite(resourceloader.load<sf::Texture, sf::TextureLoader>("res/textures/vlad/atlas.png"));
            sprite.offset = {-8.f, -16.f};
            registry.emplace<Sprite>(player, sprite);
    
        SpriteAnimation sprite_anim;
            sprite_anim.spritesheet = resourceloader.load<Spritesheet::Resource, Spritesheet::Loader>("res/textures/vlad/spritesheet.json");
            registry.emplace<SpriteAnimation>(player, sprite_anim);
    
        HealthBar healthbar;
            healthbar.offset = sprite.offset + sf::Vector2f(-4.f, -6.f);
            healthbar.size = {24.f, 3.5f};
            healthbar.color = sf::Color::Black;
            healthbar.color_empty = sf::Color::Red;
            healthbar.color_full = sf::Color::Green;
         
            registry.emplace<HealthBar>(player, healthbar);
    
        Camera player_camera;
            player_camera.view = sf::View(
                    {0.f, 0.f},
                    {
                        static_cast<float>(Singleton::Variables::WINDOW_SIZE.x) / 3.5f,
                        static_cast<float>(Singleton::Variables::WINDOW_SIZE.y) / 3.5f
                    }
                );
                 
            registry.emplace<Camera>(player, player_camera);
    }

    {auto entity = registry.create();
    auto& glued_to = registry.emplace<GluedTo>(entity, player);
    auto& z_index = registry.emplace<ZIndex>(entity, 2);
    auto& transform = registry.emplace<Transform>(entity);
    auto& sprite = registry.emplace<Sprite>(entity,
        resourceloader.load<sf::Texture, sf::TextureLoader>("res/textures/sharik/atlas.png"));
    sprite.center = true;

    auto& sprite_animation = registry.emplace<SpriteAnimation>(entity);

    sprite_animation.spritesheet = resourceloader.load<Spritesheet::Resource, Spritesheet::Loader>("res/textures/sharik/spritesheet.json");
    sprite_animation.play("idle");
    }

    {auto spawner = registry.create();
        auto& z_index = registry.emplace<ZIndex>(spawner, 1);
        auto& transform = registry.emplace<Transform>(spawner);
        auto& mob_spawner = registry.emplace<MobSpawner>(spawner); {
            mob_spawner.spawn_func = packed_entity::zobi::spawn;
            mob_spawner.cooldown = 5.5f;
            mob_spawner.spawn_range = sf::Vector2(450.f, 450.f);
            mob_spawner.spawn_soundbuffer = resourceloader.load<sf::SoundBuffer, sf::SoundBufferLoader>("res/audio/wither-spawn.mp3");
        }
    
    
        registry.emplace<Sprite>(spawner, resourceloader.load<sf::Texture, sf::TextureLoader>("res/textures/spawner/atlas.png"));
        registry.emplace<SpriteAnimation>(
            spawner,
            resourceloader.load<Spritesheet::Resource, Spritesheet::Loader>("res/textures/spawner/spritesheet.json")).play("idle");
    
    }
    

    //Floor
    {auto floor = registry.create();
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

            
            player_input_system(registry, window);
            
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                sf::Keyboard::Key code = keyPressed->code;
                
                if (code == sf::Keyboard::Key::F3) {
                    debug_text.set_visibility(!debug_text.is_visible());
                }
                
                if (code == sf::Keyboard::Key::F6) {
                    debug_hitboxes = !debug_hitboxes;
                }
            }
            
            
            Console::get_instance().handle_event(*event, window);
        }

        sf::Time elapsed = clock.restart();
        float delta_time = elapsed.asSeconds() * speed_scale;

        health_system(registry);
        attack_system_manager_handler(registry, delta_time);
        
        AISystems::update(registry, delta_time);
        mob_system(registry, delta_time);
        mob_spawner_system(registry, delta_time);
        
        movement_system(registry, delta_time);
        glue_system(registry);
        
        projectile_system(registry, delta_time);
        
        camera_system(registry, window, delta_time);

        sprite_animation_control_system(registry);
        sprite_animation_system(registry, delta_time);

        Console::get_instance().update(window, delta_time);

        window.clear(sf::Color::Blue);

        render_system(registry, window);
        
        window.setView(window.getDefaultView()); 
        
        ui_render_system(registry, window);
        
        debug_text.update(registry, delta_time);
        debug_text.render(window);

        Console::get_instance().render(window);

        window.display();
    }

    std::cout << termcolor::yellow << "Game Closed" << termcolor::reset << std::endl;
    return 0;
}

