#pragma once

#include "ItemData.hpp"
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>


class ItemRegistry {
public:
    void register_item(const std::string& path) {
        
    }

    const ItemData* get(const std::string& id) const {
        auto it = registry.find(id);
        return (it != registry.end()) ? &it->second : nullptr;
    }

private:
    std::unordered_map<std::string, ItemData> registry;
};
