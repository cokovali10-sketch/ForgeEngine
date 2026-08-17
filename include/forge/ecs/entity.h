#pragma once

#include <cstdint>

namespace forge::ecs {

using EntityId = std::uint32_t;
using EntityIndex = std::uint32_t;
using EntityVersion = std::uint32_t;

// Compact 64-bit entity handle: 32-bit index + 32-bit version.
// The version guards against stale references after entity reuse.
struct Entity {
    EntityIndex index = 0;
    EntityVersion version = 0;

    constexpr Entity() = default;
    constexpr Entity(EntityIndex i, EntityVersion v) : index(i), version(v) {}

    static constexpr Entity none() { return {0xFFFFFFFFu, 0}; }

    constexpr bool operator==(const Entity& o) const {
        return index == o.index && version == o.version;
    }
    constexpr bool operator!=(const Entity& o) const { return !(*this == o); }
    constexpr bool valid() const { return index != 0xFFFFFFFFu; }
};

}  // namespace forge::ecs
