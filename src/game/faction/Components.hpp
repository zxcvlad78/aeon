#pragma once
#include "../../../include/Components.hpp"

struct Faction {
    std::string name;
};

struct AlliedFactions {
    std::vector<std::string> list;
};

struct EnemyFactions {
    std::vector<std::string> list;
};