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
        std::size_t h1 = std::hash<int>()(p.first);
        std::size_t h2 = std::hash<int>()(p.second);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
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

    void insert(Entity entity, const sf::Vector2f& center, float radius) {
        sf::FloatRect aabb;
        aabb.position = {center.x - radius, center.y - radius};
        aabb.size = {radius * 2.f, radius * 2.f};
        insert(entity, aabb);
    }

    void insert(Entity entity, const sf::Vector2f& position, const Hitbox& hitbox) {
        if (hitbox.radius > 0.f) {
            sf::Vector2f center = position + hitbox.offset;
            sf::FloatRect aabb;
            aabb.position = {center.x - hitbox.radius, center.y - hitbox.radius};
            aabb.size = {hitbox.radius * 2.f, hitbox.radius * 2.f};
            insert(entity, aabb);
        } else {
            sf::FloatRect aabb;
            aabb.position = {position.x + hitbox.offset.x, position.y + hitbox.offset.y};
            aabb.size = hitbox.size;
            insert(entity, aabb);
        }
    }

    void query(const sf::FloatRect& aabb, std::vector<Entity>& out_candidates) const {
        out_candidates.clear();

        auto [x_min, y_min] = get_cell_coords(aabb.position.x, aabb.position.y);
        auto [x_max, y_max] = get_cell_coords(aabb.position.x + aabb.size.x, aabb.position.y + aabb.size.y);

        for (int x = x_min; x <= x_max; ++x) {
            for (int y = y_min; y <= y_max; ++y) {
                auto it = cells_.find({x, y});
                if (it != cells_.end() && !it->second.empty()) {
                    out_candidates.insert(out_candidates.end(), it->second.begin(), it->second.end());
                }
            }
        }

        std::sort(out_candidates.begin(), out_candidates.end());
        auto last = std::unique(out_candidates.begin(), out_candidates.end());
        out_candidates.erase(last, out_candidates.end());
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