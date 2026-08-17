#pragma once

#include "forge/ecs/entity.h"

#include <cstddef>
#include <unordered_map>
#include <vector>

namespace forge::ecs {

// Sparse-set storage for a single component type.
// O(1) add/remove/get, packed iteration over dense data.
template <typename T>
class ComponentStorage {
public:
    using DenseVector = std::vector<T>;
    using EntityVector = std::vector<EntityIndex>;

    T* try_get(EntityIndex idx) {
        if (idx < sparse_.size() && sparse_[idx] < dense_.size() &&
            entities_[sparse_[idx]] == idx) {
            return &dense_[sparse_[idx]];
        }
        return nullptr;
    }

    const T* try_get(EntityIndex idx) const {
        return const_cast<ComponentStorage*>(this)->try_get(idx);
    }

    T& emplace(EntityIndex idx, T&& value) {
        if (T* existing = try_get(idx)) {
            *existing = std::move(value);
            return *existing;
        }
        if (idx >= sparse_.size()) {
            sparse_.resize(idx + 1, 0xFFFFFFFFu);
        }
        sparse_[idx] = static_cast<EntityIndex>(dense_.size());
        dense_.push_back(std::move(value));
        entities_.push_back(idx);
        return dense_.back();
    }

    bool remove(EntityIndex idx) {
        auto* pos = try_get(idx);
        if (!pos) {
            return false;
        }
        EntityIndex last = static_cast<EntityIndex>(dense_.size() - 1);
        EntityIndex hole = sparse_[idx];
        EntityIndex moved = entities_[last];
        dense_[hole] = std::move(dense_[last]);
        entities_[hole] = moved;
        sparse_[moved] = hole;
        dense_.pop_back();
        entities_.pop_back();
        sparse_[idx] = 0xFFFFFFFFu;
        return true;
    }

    void clear() {
        dense_.clear();
        entities_.clear();
        sparse_.clear();
    }

    std::size_t size() const { return dense_.size(); }
    bool empty() const { return dense_.empty(); }

    const DenseVector& dense() const { return dense_; }
    DenseVector& dense() { return dense_; }
    const EntityVector& entities() const { return entities_; }

private:
    std::vector<EntityIndex> sparse_;
    DenseVector dense_;
    EntityVector entities_;
};

}  // namespace forge::ecs
