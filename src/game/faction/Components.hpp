#pragma once
#include "../../../include/Components.hpp"

struct Faction {
    std::string id;
};

struct AlliedFactions {
    std::vector<std::string> list;
    void add(const std::string& faction_id) {
        list.push_back(faction_id);
    }
};

struct EnemyFactions {
    std::vector<std::string> list;
    void add(const std::string& faction_id) {
        list.push_back(faction_id);
    }
};