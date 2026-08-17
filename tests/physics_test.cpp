#include <cassert>
#include <cmath>
#include <iostream>

#include "forge/math/vec2.h"
#include "forge/physics/aabb.h"

int main() {
    // Vec2 basics.
    forge::Vec2 a{3.0f, 4.0f};
    assert(std::fabs(a.length() - 5.0f) < 1e-5f);
    forge::Vec2 n = a.normalized();
    assert(std::fabs(n.length() - 1.0f) < 1e-5f);
    forge::Vec2 sum = a + forge::Vec2{1.0f, 1.0f};
    assert(sum.x == 4.0f && sum.y == 5.0f);
    forge::Vec2 dot_x{1.0f, 0.0f};
    assert(a.dot(dot_x) == 3.0f);

    // Rect overlap.
    forge::Rect r1{0.0f, 0.0f, 10.0f, 10.0f};
    forge::Rect r2{5.0f, 5.0f, 10.0f, 10.0f};
    assert(r1.overlaps(r2));
    forge::Vec2 p_in{2.0f, 2.0f};
    forge::Vec2 p_out{10.0f, 10.0f};
    assert(r1.contains(p_in));
    assert(!r1.contains(p_out));

    // AABB resolution: left-side push-out.
    forge::Rect player{5.0f, 5.0f, 4.0f, 4.0f};   // moving right into
    forge::Rect wall{10.0f, 0.0f, 10.0f, 20.0f};  // vertical wall
    auto c = forge::physics::resolve_rect(player, wall);
    assert(c.collided);
    assert(c.normal.x > 0.5f);  // pushed back to the left
    assert(c.penetration > 0.0f);

    // No overlap -> no collision.
    forge::Rect far{100.0f, 100.0f, 4.0f, 4.0f};
    assert(!forge::physics::resolve_rect(far, wall).collided);

    std::cout << "physics_test: all assertions passed\n";
    return 0;
}