#include "Systems.hpp"

void render_tilemap(entt::registry& registry, sf::RenderWindow& window) {
    auto view = registry.view<TileMap>();

    for (auto [entity, tilemap] : view.each()) {
        if (!tilemap.tileset) continue;
        if (tilemap.tiles.empty()) continue;

        sf::VertexArray vertices(sf::PrimitiveType::Triangles, tilemap.width * tilemap.height * 6);

        unsigned short tile_size = tilemap.tileset->tile_size;
        sf::Vector2u texture_size = tilemap.tileset->size();
        unsigned int tiles_per_row = texture_size.x / tile_size;

        for (unsigned int y = 0; y < tilemap.height; ++y) {
            for (unsigned int x = 0; x < tilemap.width; ++x) {
                int index = y * tilemap.width + x;
                int tile_id = tilemap.tiles[index];
                if (tile_id < 0) continue;

                unsigned int tx = tile_id % tiles_per_row;
                unsigned int ty = tile_id / tiles_per_row;

                int left   = static_cast<int>(tx * tile_size);
                int top    = static_cast<int>(ty * tile_size);
                int width  = static_cast<int>(tile_size);
                int height = static_cast<int>(tile_size);

                sf::Vector2f pos(static_cast<float>(x * tile_size),
                                 static_cast<float>(y * tile_size));

                unsigned int tri = index * 6;

                vertices[tri + 0].position = pos;
                vertices[tri + 0].texCoords = sf::Vector2f(static_cast<float>(left),
                                                           static_cast<float>(top));
  
                vertices[tri + 1].position = pos + sf::Vector2f(tile_size, 0.f);
                vertices[tri + 1].texCoords = sf::Vector2f(static_cast<float>(left + width),
                                                           static_cast<float>(top));

                vertices[tri + 2].position = pos + sf::Vector2f(0.f, tile_size);
                vertices[tri + 2].texCoords = sf::Vector2f(static_cast<float>(left),
                                                           static_cast<float>(top + height));

                vertices[tri + 3].position = pos + sf::Vector2f(tile_size, 0.f);
                vertices[tri + 3].texCoords = sf::Vector2f(static_cast<float>(left + width),
                                                           static_cast<float>(top));

                vertices[tri + 4].position = pos + sf::Vector2f(tile_size, tile_size);
                vertices[tri + 4].texCoords = sf::Vector2f(static_cast<float>(left + width),
                                                           static_cast<float>(top + height));

                vertices[tri + 5].position = pos + sf::Vector2f(0.f, tile_size);
                vertices[tri + 5].texCoords = sf::Vector2f(static_cast<float>(left),
                                                           static_cast<float>(top + height));
            } 
        }

        sf::RenderStates states;
        states.texture = tilemap.tileset->texture.operator->(); //baagiii
        window.draw(vertices, states);
    }
}