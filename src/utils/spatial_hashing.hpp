#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include <cmath>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <entt/entity/entity.hpp>

struct PairHash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

class SpatialHash {
public:
    using Entity = entt::entity;
    using CellKey = std::pair<int, int>;

    explicit SpatialHash(float cell_size) : cell_size_(cell_size) {}

    void clear() {
        cells_.clear();
    }

    void insert(Entity entity, const sf::FloatRect& aabb) {
        float left   = aabb.position.x;
        float top    = aabb.position.y;
        float right  = aabb.position.x + aabb.size.x;
        float bottom = aabb.position.y + aabb.size.y;

        auto [x_min, y_min] = get_cell_coords(left, top);
        auto [x_max, y_max] = get_cell_coords(right, bottom);

        for (int x = x_min; x <= x_max; ++x) {
            for (int y = y_min; y <= y_max; ++y) {
                cells_[{x, y}].push_back(entity);
            }
        }
    }

    std::vector<Entity> query(const sf::FloatRect& aabb) const {
        float left   = aabb.position.x;
        float top    = aabb.position.y;
        float right  = aabb.position.x + aabb.size.x;
        float bottom = aabb.position.y + aabb.size.y;

        auto [x_min, y_min] = get_cell_coords(left, top);
        auto [x_max, y_max] = get_cell_coords(right, bottom);

        std::unordered_set<Entity> unique;
        for (int x = x_min; x <= x_max; ++x) {
            for (int y = y_min; y <= y_max; ++y) {
                auto it = cells_.find({x, y});
                if (it != cells_.end()) {
                    for (Entity e : it->second) {
                        unique.insert(e);
                    }
                }
            }
        }

        return std::vector<Entity>(unique.begin(), unique.end());
    }

private:
    float cell_size_;
    std::unordered_map<CellKey, std::vector<Entity>, PairHash> cells_;

    std::pair<int, int> get_cell_coords(float x, float y) const {
        int cx = static_cast<int>(std::floor(x / cell_size_));
        int cy = static_cast<int>(std::floor(y / cell_size_));
        return {cx, cy};
    }
};