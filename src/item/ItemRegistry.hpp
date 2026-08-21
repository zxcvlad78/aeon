#pragma once

#include "Item.hpp"
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>


class ItemRegistry {
public:
    void register_item(const std::string& path) {
        
    }
    void register_item(ItemData& item_data) {
        registry.emplace(item_data.id, item_data);
    }

    const ItemData* get(const std::string& id) const {
        auto it = registry.find(id);
        return (it != registry.end()) ? &it->second : nullptr;
    }

private:
    std::unordered_map<std::string, ItemData> registry;
};
