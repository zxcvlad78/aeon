#pragma once

#include <Components.hpp>
#include "../systems/ComponentSystems.hpp"
#include "../game/singleton/singleton.hpp"
#include "console/Console.hpp"

#include "../game/render/Systems.hpp"
#include "../game/ai/Systems.hpp"
#include "../game/mob/Systems.hpp"
#include "../game/mob_spawner/Systems.hpp"

namespace ConsoleCommands {
    inline void init(sf::RenderWindow& window, entt::registry& registry) {
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
            "fps_max <int>"
        );
        Console::get_instance().register_command(
            "volume",
            [&window](const std::vector<std::string>& args) {
                if (!args.empty()) {
                    try {
                        //float vol = std::stoi(args[0]);
                        sf::Listener::setGlobalVolume(std::stoi(args[0]));
                    } catch (const std::exception& e) {
                        Console::get_instance().print_error(e.what());
                    }
                }
            },
            "Set audio volume (0.0-100.0)",
            "volume <float>"
        );
        Console::get_instance().register_command(
            "player.invulnerable",
            [&registry](const std::vector<std::string>& args) {
                if (!args.empty()) {
                    try {
                        int val = std::stoi(args[0]); bool invulnerable = val > 0;
                        
                        if (invulnerable) {
                            for (auto [e] : registry.view<Player>().each()) {
                                registry.emplace_or_replace<Invulnerable>(e);
                            }
                        } else {
                            for (auto [e] : registry.view<Player>().each()) {
                                if (registry.all_of<Invulnerable>(e))
                                    registry.remove<Invulnerable>(e);
                            }
                        }
                        
                        Console::get_instance().print_success("player.invulnerable: " + invulnerable);
                    } catch (const std::exception& e) {
                        Console::get_instance().print_error(e.what());
                    }
                }
            },
            "Set player invulnerable enabled",
            "player.invulnerable <bool>"
        );
        Console::get_instance().register_command(
            "collision.enabled",
            [](const std::vector<std::string>& args) {
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
            "collision.enabled <bool>"
        );
        Console::get_instance().register_command(
            "render.enabled",
            [](const std::vector<std::string>& args) {
                if (!args.empty()) {
                    try {
                        int val = std::stoi(args[0]);
                        RenderSystems::enabled = val > 0;
                        Console::get_instance().print_success("Render enabled: " + std::to_string(RenderSystems::enabled));
                    } catch (const std::exception& e) {
                        Console::get_instance().print_error(e.what());
                    }
                }
            },
            "Set render enabled",
            "render.enabled <bool>"
        );
        Console::get_instance().register_command(
            "ai.enabled",
            [](const std::vector<std::string>& args) {
                if (!args.empty()) {
                    try {
                        int val = std::stoi(args[0]);
                        AISystems::enabled = val > 0;
                        Console::get_instance().print_success("AI enabled: " + std::to_string(AISystems::enabled));
                    } catch (const std::exception& e) {
                        Console::get_instance().print_error(e.what());
                    }
                }
            },
            "Set AI enabled",
            "ai.enabled <bool>"
        );
        Console::get_instance().register_command(
            "mob.enabled",
            [](const std::vector<std::string>& args) {
                if (!args.empty()) {
                    try {
                        int val = std::stoi(args[0]);
                        MobSystems::enabled = val > 0;
                        Console::get_instance().print_success("mob enabled: " + std::to_string(MobSystems::enabled));
                    } catch (const std::exception& e) {
                        Console::get_instance().print_error(e.what());
                    }
                }
            },
            "Set mob enabled",
            "mob.enabled <bool>"
        );
        Console::get_instance().register_command(
            "speed",
            [](const std::vector<std::string>& args) {
                if (!args.empty()) {
                    try {
                        float val = std::stof(args[0]);
                        if (val >= 0.f) {
                            Singleton::Variables::speed_scale = val;
                            Console::get_instance().print_success("Speed scale set to: " + std::to_string(val));
                        } else {
                            Console::get_instance().print_error("Speed scale must be positive");
                        }
                    } catch (const std::exception& e) {
                        Console::get_instance().print_error(e.what());
                    }
                } else {
                    Console::get_instance().print_success("Current speed scale: " + std::to_string(Singleton::Variables::speed_scale));
                }
            },
            "Set time speed multiplier",
            "speed <float>"
        );
        Console::get_instance().register_command(
            "mob.spawn.enabled",
            [](const std::vector<std::string>& args) {
                if (!args.empty()) {
                    try {
                        int val = std::stoi(args[0]);
                        MobSpawnerSystems::enabled = val > 0;
                        Console::get_instance().print_success("mob.spawn enabled: " + std::to_string(MobSpawnerSystems::enabled));
                    } catch (const std::exception& e) {
                        Console::get_instance().print_error(e.what());
                    }
                }
            },
            "Set mob spawn enabled",
            "mob.spawn.enabled <bool>"
        );
        Console::get_instance().register_command(
            "player.respawn",
            [&registry](const std::vector<std::string>& args) {
                try {
                    for (auto [e] : registry.view<Player>().each()) {
                        registry.destroy(e);
                    }
                    packed_entity::player::spawn(registry);
                } catch (const std::exception& e) {
                    Console::get_instance().print_error(e.what());
                }
            },
            "Respawn player",
            "player.respawn"
        );
        Console::get_instance().register_command(
            "player.kill",
            [&registry](const std::vector<std::string>& args) {
                try {
                    for (auto [e, h] : registry.view<Player, Health>().each()) {
                        h.apply_damage(h.get_max_value());
                    }
                } catch (const std::exception& e) {
                    Console::get_instance().print_error(e.what());
                }
            },
            "Kill player",
            "player.kill"
        );
        Console::get_instance().register_command(
            "player.destroy",
            [&registry](const std::vector<std::string>& args) {
                try {
                    for (auto [e] : registry.view<Player>().each()) {
                        registry.destroy(e);
                    }
                } catch (const std::exception& e) {
                    Console::get_instance().print_error(e.what());
                }
            },
            "Destroy player",
            "player.destroy"
        );

        // Test
        // for (uint8_t i = 0; i < 25; i ++) {
        //     std::string str_i = std::to_string(i);
        //     Console::get_instance().register_command(
        //         "sas" + str_i,
        //         [](const std::vector<std::string>& args) { },
        //         "pro100 sas nomer " + str_i,
        //         "nikak))"
        //     );
        // }
    }
}