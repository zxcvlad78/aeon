#pragma once
#include "../../../include/Components.hpp"


struct TileMap {
    unsigned int width;
    unsigned int height;
    
    std::vector<int> tiles;
    TileSet::Resource* tile_set = nullptr;
};