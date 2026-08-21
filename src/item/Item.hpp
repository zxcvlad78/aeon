#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <variant>
#include <entt/entt.hpp>

struct ItemData {
    std::string id;
    std::string name;
    std::string description;
    uint32_t max_stack = 64;
    
    std::vector<std::string> tags; 
};

struct Item {
    std::string id;
    std::unordered_map<std::string, std::variant<int, float, std::string>> properties;
};

struct Weapon {
    float damage;
    float attack_speed;
};

struct Consumable {
    int sasas;
};

struct ItemStack {
    entt::entity entity = entt::null;
    uint32_t count = 0;

    bool is_empty() const { return entity == entt::null || count == 0; }
};


struct ItemContainer {
    std::vector<ItemStack> slots;
    uint32_t capacity;

};

struct Durability {
private:
    unsigned int value = 100;
    unsigned int max_value = 100;

public:
    void set_value(unsigned int new_value) {
        value = std::clamp(new_value, std::numeric_limits<unsigned int>::lowest(), max_value);
    } unsigned int get_value() const { return value; }

    void set_max_value(unsigned int new_max_value) {
        max_value = new_max_value;
    } unsigned int get_max_value() const { return max_value; }
};
