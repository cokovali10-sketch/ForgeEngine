#pragma once

#include <cstdint>

#include "forge/math/vec2.h"

namespace forge {

struct Rect {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;

    constexpr Rect() = default;
    constexpr Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}

    constexpr float left() const { return x; }
    constexpr float right() const { return x + w; }
    constexpr float top() const { return y; }
    constexpr float bottom() const { return y + h; }

    constexpr Vec2 center() const { return {x + w * 0.5f, y + h * 0.5f}; }

    constexpr bool contains(const Vec2& p) const {
        return p.x >= left() && p.x < right() && p.y >= top() && p.y < bottom();
    }

    constexpr bool overlaps(const Rect& o) const {
        return left() < o.right() && right() > o.left() &&
               top() < o.bottom() && bottom() > o.top();
    }
};

struct Color {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;

    constexpr Color() = default;
    constexpr Color(float r, float g, float b, float a = 1.0f)
        : r(r), g(g), b(b), a(a) {}

    static constexpr Color black() { return {0.0f, 0.0f, 0.0f}; }
    static constexpr Color white() { return {1.0f, 1.0f, 1.0f}; }
    static constexpr Color red() { return {1.0f, 0.0f, 0.0f}; }
    static constexpr Color green() { return {0.0f, 1.0f, 0.0f}; }
    static constexpr Color blue() { return {0.0f, 0.0f, 1.0f}; }
    static constexpr Color from_rgb255(uint8_t r, uint8_t g, uint8_t b) {
        return {r / 255.0f, g / 255.0f, b / 255.0f};
    }
};

}  // namespace forge
