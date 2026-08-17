#pragma once

#include "forge/math/rect.h"
#include "forge/math/vec2.h"

namespace forge {

struct Sprite {
    Rect rect;            // world-space rect
    Color color = Color::white();
    bool solid = false;   // participates in AABB collisions
    bool flip_x = false;
    bool flip_y = false;
};

}  // namespace forge