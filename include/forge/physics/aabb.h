#pragma once

#include "forge/math/rect.h"
#include "forge/math/vec2.h"

namespace forge::physics {

// Axis-aligned bounding box collision helpers.

struct Contact {
    bool collided = false;
    Vec2 normal;   // push-out direction (normalized)
    float penetration = 0.0f;
};

inline Contact resolve_rect(const Rect& a, const Rect& b) {
    if (!a.overlaps(b)) {
        return {};
    }
    float push_left = a.right() - b.left();
    float push_right = b.right() - a.left();
    float push_top = a.bottom() - b.top();
    float push_bottom = b.bottom() - a.top();

    float min_x = std::min(push_left, push_right);
    float min_y = std::min(push_top, push_bottom);

    Contact c;
    c.collided = true;
    if (min_x < min_y) {
        c.normal = push_left < push_right ? Vec2{-1.0f, 0.0f} : Vec2{1.0f, 0.0f};
        c.penetration = min_x;
    } else {
        c.normal = push_top < push_bottom ? Vec2{0.0f, -1.0f} : Vec2{0.0f, 1.0f};
        c.penetration = min_y;
    }
    return c;
}

}  // namespace forge::physics