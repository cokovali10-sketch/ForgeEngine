#pragma once

#include "forge/math/rect.h"
#include "forge/math/vec2.h"

namespace forge {

struct Transform {
    Vec2 position;
    Vec2 scale{1.0f, 1.0f};
    float rotation = 0.0f;

    Transform() = default;
    explicit Transform(const Vec2& pos) : position(pos) {}
    Transform(const Vec2& pos, const Vec2& scl, float rot = 0.0f)
        : position(pos), scale(scl), rotation(rot) {}
};

struct Velocity {
    Vec2 value;
};

struct RigidBody {
    Vec2 velocity;
    bool gravity = false;
};

}  // namespace forge