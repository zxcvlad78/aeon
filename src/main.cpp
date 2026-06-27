#include <termcolor/termcolor.hpp>
#include "ResourceLoader.hpp"
#include "SoundPlayer.hpp"
#include "systems/ComponentSystems.hpp"
#include "systems/UIComponentSystems.hpp"

#include "game/mob/Systems.hpp"

#include "utils/DebugText.hpp"
#include "utils/math.hpp"

const std::string GAME_VERSION = "v0.0.1";

const sf::Vector2u WINDOW_SIZE = sf::Vector2u(800, 800);

entt::entity spawn_projectile(
    entt::registry& registry,
    Projectile      projectile,
    Velocity        velocity,
    std::string     atlas_path = "res/textures/t_projectile/atlas.png",
    std::string     spritesheet_path = "res/t_projectile_spritesheet.json") {
    
    auto projectile_entity = registry.create();
        registry.emplace<ZIndex>(projectile_entity, 2);
        registry.emplace<Projectile>(projectile_entity, projectile);
        registry.emplace<Velocity>(projectile_entity, velocity);

    Transform projectile_transform;
        projectile_transform.position = {100.f, 100.f};
        registry.emplace<Transform>(projectile_entity, projectile_transform);
    

    return projectile_entity;
}

entt::entity spawn_enemy(
    entt::registry& registry,
    std::string projectile_atlas_path,
    std::string projectile_spritesheet_path
) {
    auto entity = registry.create();
    registry.emplace<ZIndex>(entity, 1);
    registry.emplace<Transform>(entity);
    registry.emplace<Velocity>(entity, 0.f, 0.f, true);
    registry.emplace<SpriteAnimationControl>(entity);
    
    registry.emplace<Health>(entity, 100.f, 100.f);
    registry.emplace<MoveSpeed>(entity, 100.0f);
    
    registry.emplace<Mob>(entity);

    Attack attack;
        registry.emplace<Attack>(entity, attack);

    MobAttackRanged mob_attack_ranged;
        mob_attack_ranged.projectile = Projectile();
        mob_attack_ranged.initial_velocity = Velocity(100.f, 100.f);
        mob_attack_ranged.projectile_atlas_path = projectile_atlas_path;
        mob_attack_ranged.projectile_spritesheet_path = projectile_spritesheet_path;

        registry.emplace<MobAttackRanged>(entity, mob_attack_ranged);

    Hitbox hitbox;
        hitbox.size = {16.f, 32.f};
        hitbox.offset = {-hitbox.size.x / 2.f, -hitbox.size.y / 2.f};
        registry.emplace<Hitbox>(entity, hitbox);

    Sprite sprite(resourceloader.load<sf::Texture, sf::TextureLoader>("res/textures/zloipacan/atlas.png"));
        sprite.offset = {-8.f, -16.f};
        registry.emplace<Sprite>(entity, sprite);
    
    SpriteAnimation sprite_anim;
        sprite_anim.spritesheet = resourceloader.load<Spritesheet::Resource, Spritesheet::Loader>("res/textures/zloipacan/spritesheet.json");
        registry.emplace<SpriteAnimation>(entity, sprite_anim);

    HealthBar healthbar;
        healthbar.offset = sprite.offset + sf::Vector2f(-4.f, -6.f);
        healthbar.size = {24.f, 3.5f};
        healthbar.color = sf::Color::Black;
        healthbar.color_empty = sf::Color::Red;
        healthbar.color_full = sf::Color::Green;
        
        registry.emplace<HealthBar>(entity, healthbar);

    return entity;
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
    {auto player = registry.create();
        registry.emplace<ZIndex>(player, 1);
        registry.emplace<Transform>(player);
        registry.emplace<Velocity>(player, 0.f, 0.f, true);
        registry.emplace<SpriteAnimationControl>(player);
        registry.emplace<PlayerInput>(player);
        
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
                    {static_cast<float>(WINDOW_SIZE.x) / 4.f, static_cast<float>(WINDOW_SIZE.y) / 4.f}
                );
                    
            registry.emplace<Camera>(player, player_camera);
    }
    
    auto mob_enemy = spawn_enemy(
        registry,
        "res/textures/zloipacan/atlas.png",
        "res/textures/zloipacan/spritesheet.json"
    );
    registry.get<Transform>(mob_enemy).position = {60.f, 100.f};

    spawn_projectile(
        registry,
        Projectile(),
        Velocity(-100.f, -100.f)
    );

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
                    spawn_projectile(
                        registry,
                        Projectile(),
                        Velocity(-100.f, -100.f)
                    );
                }
            }
            
        }

        sf::Time elapsed = clock.restart();
        float delta_time = elapsed.asSeconds();


        player_input_system(registry);
        mob_system(registry, delta_time);
        movement_system(registry, delta_time);
        projectile_system(registry, delta_time);
        
        camera_system(registry, window, delta_time);

        sprite_animation_control_system(registry);
        sprite_animation_system(registry, delta_time);

        window.clear(sf::Color::Blue);

        render_system(registry, window);
        
        window.setView(window.getDefaultView()); 
        
        ui_render_system(registry, window);
        
        debug_text.update(delta_time);
        debug_text.render(window);

        window.display();
    }

    std::cout << termcolor::yellow << "end" << termcolor::reset << std::endl;
    return 0;
}

