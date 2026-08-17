#include "forge/math/vec2.h"

#include <cmath>

namespace forge {

float Vec2::length() const {
    return std::sqrt(lengthSq());
}

Vec2 Vec2::normalized() const {
    float len = length();
    if (len < 1e-6f) {
        return {0.0f, 0.0f};
    }
    return *this / len;
}

}  // namespace forge
