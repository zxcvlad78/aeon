#include <termcolor/termcolor.hpp>
#include "ResourceLoader.hpp"
#include "SoundPlayer.hpp"
#include "game/singleton/singleton.hpp"

#include "game/packed_entity/general.h"

#include "systems/ComponentSystems.hpp"
#include "systems/UIComponentSystems.hpp"

#include <Aeon.hpp>

#include "utils/DebugText.hpp"
#include "utils/math.hpp"
#include <ctime>

#include "utils/rng.hpp"

#include "console/Console.hpp"
#include "console/commands.hpp"

bool collision_enabled = true;
bool AISystems::enabled = true;
bool MobSystems::enabled = true;
bool MobSpawnerSystems::enabled = true;
bool RenderSystems::enabled = true;
bool RenderSystems::draw_hitboxes_enabled = false;

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
    ConsoleCommands::init(window);

    DebugText debug_text(Singleton::Variables::main_font);
    
    entt::registry registry;

    auto paren = packed_entity::gad::spawn(registry); {
        if (auto* t = registry.try_get<Transform>(paren)) {
            t->position = sf::Vector2{85.f, 25.f};
        }
        if (auto* anim = registry.try_get<SpriteAnimation>(paren)) {
            anim->play("idle");
        }
    }
    

    //Player
    auto player = registry.create();
    {
        registry.emplace<ZIndex>(player, 1);
        registry.emplace<Transform>(player).position = {50.f , 50.f};
        registry.emplace<Velocity>(player, 0.f, 0.f, true);
        registry.emplace<SpriteAnimationControl>(player);
        registry.emplace<PlayerInput>(player);
        
        auto& attack = registry.emplace<Attack>(player); {
            attack.spawn_func = packed_entity::default_projectile::spawn;
            attack.interval = 0.2f; 
        }
        
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
                        static_cast<float>(Singleton::Variables::WINDOW_SIZE.x) / 3.0f,
                        static_cast<float>(Singleton::Variables::WINDOW_SIZE.y) / 3.0f
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

    auto spawner_1 = packed_entity::mob_spawner::spawn(
        registry,
        packed_entity::zobi::spawn
    );

    auto spawner_2 = packed_entity::mob_spawner::spawn(
        registry,
        packed_entity::gad::spawn
    );
    

    //Test (sex)
    auto vector2_testing = registry.create(); {
        registry.emplace<Transform>(vector2_testing);
        registry.emplace<Vector2Testing>(vector2_testing);
    }
    

    // TileMap
    {auto entity = registry.create();
        registry.emplace<Transform>(entity);
        //registry.emplace<GluedTo>(entity, vector2_testing); // teeest)))
        auto& tilemap = registry.emplace<TileMap>(entity); {
            tilemap.tileset = resourceloader.load<TileSet::Resource, TileSet::Loader>("res/tileset.json");
            tilemap.load_tiles("res/tilemap.json");
        }
    }

    //twotaunt
    for (int i = 1; i <= 3; i++) {
        packed_entity::twotaunt::spawn(registry, static_cast<float>(i));
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
                    RenderSystems::draw_hitboxes_enabled = !RenderSystems::draw_hitboxes_enabled;
                }
            }
            
            Console::get_instance().handle_event(*event, window);
        }

        sf::Time elapsed = clock.restart();
        float delta_time = elapsed.asSeconds();
        float scaled_delta_time = delta_time * Singleton::Variables::speed_scale;

        health_system(registry);
        attack_system_manager_handler(registry, scaled_delta_time);
        
        AISystems::update(registry, scaled_delta_time);
        MobSystems::update(registry, scaled_delta_time);
        MobSpawnerSystems::update(registry, scaled_delta_time);
        
        vector2_testing_system(registry, scaled_delta_time);

        movement_system(registry, scaled_delta_time);
        glue_system(registry);
        
        CameraSystems::update(registry, window, scaled_delta_time);

        projectile_system(registry, scaled_delta_time);
        
        sprite_animation_control_system(registry);
        SpriteSystems::update(registry, window, scaled_delta_time);

        Console::get_instance().update(window, delta_time);

        window.clear(sf::Color::Black);

        render_tilemap(registry, window);
        RenderSystems::update(registry, window);
        
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

