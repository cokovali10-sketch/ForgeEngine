#pragma once

#include "forge/ecs/entity.h"
#include "forge/ecs/storage.h"

#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace forge::ecs {

namespace detail {

struct IStorageBase {
    virtual ~IStorageBase() = default;
    virtual void erase(EntityIndex idx) = 0;
};

template <typename T>
struct TypedStorage : IStorageBase {
    ComponentStorage<T> storage;
    void erase(EntityIndex idx) override { storage.remove(idx); }
};

}  // namespace detail

// Minimal but complete ECS world.
// - Entities: integer handles with generation counters.
// - Components: typed sparse-set storages, O(1) CRUD.
class World {
public:
    World() = default;
    World(const World&) = delete;
    World& operator=(const World&) = delete;

    // --- Entities ------------------------------------------------------

    Entity create() {
        EntityIndex idx;
        EntityVersion ver;
        if (!free_.empty()) {
            idx = free_.back();
            free_.pop_back();
            ver = versions_[idx];
        } else {
            idx = static_cast<EntityIndex>(versions_.size());
            versions_.push_back(1);
            ver = 1;
        }
        Entity e{idx, ver};
        entities_.push_back(e);
        return e;
    }

    bool alive(Entity e) const {
        return e.index < versions_.size() && versions_[e.index] == e.version;
    }

    void destroy(Entity e) {
        if (!alive(e)) {
            return;
        }
        for (auto& [type, holder] : storages_) {
            holder->erase(e.index);
        }
        for (std::size_t i = 0; i < entities_.size(); ++i) {
            if (entities_[i] == e) {
                entities_[i] = entities_.back();
                entities_.pop_back();
                break;
            }
        }
        versions_[e.index] += 1;
        free_.push_back(e.index);
    }

    const std::vector<Entity>& entities() const { return entities_; }

    // --- Components ----------------------------------------------------

    template <typename T, typename... Args>
    T& add(Entity e, Args&&... args) {
        return storage<T>().storage.emplace(e.index, T{std::forward<Args>(args)...});
    }

    template <typename T>
    T* get(Entity e) {
        if (!alive(e)) {
            return nullptr;
        }
        return storage<T>().storage.try_get(e.index);
    }

    template <typename T>
    const T* get(Entity e) const {
        return const_cast<World*>(this)->get<T>(e);
    }

    template <typename T>
    void remove(Entity e) {
        if (auto it = storages_.find(std::type_index(typeid(T))); it != storages_.end()) {
            it->second->erase(e.index);
        }
    }

    template <typename T>
    void each(const std::function<void(Entity, T&)>& fn) {
        auto& s = storage<T>().storage;
        for (std::size_t i = 0; i < s.size(); ++i) {
            Entity e{s.entities()[i], versions_[s.entities()[i]]};
            fn(e, s.dense()[i]);
        }
    }

private:
    template <typename T>
    detail::TypedStorage<T>& storage() {
        auto key = std::type_index(typeid(T));
        if (auto it = storages_.find(key); it != storages_.end()) {
            return static_cast<detail::TypedStorage<T>&>(*it->second);
        }
        auto ptr = std::make_unique<detail::TypedStorage<T>>();
        auto* raw = ptr.get();
        storages_[key] = std::move(ptr);
        return *raw;
    }

    std::vector<Entity> entities_;
    std::vector<EntityVersion> versions_;
    std::vector<EntityIndex> free_;
    std::unordered_map<std::type_index, std::unique_ptr<detail::IStorageBase>> storages_;
};

}  // namespace forge::ecs