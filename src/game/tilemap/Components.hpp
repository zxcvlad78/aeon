#pragma once
#include "../../../include/Components.hpp"


struct TileMap {
    unsigned int width;
    unsigned int height;
    
    std::vector<int> tiles;
    entt::resource<TileSet::Resource> tileset;

    void load_tiles(const std::string& path) {
        json data = get_json_data(path);
        if (data.contains("width")) width = data["width"];
        if (data.contains("height")) height = data["height"];
        if (data.contains("tiles")) tiles = data["tiles"].get<std::vector<int>>();
    }
};