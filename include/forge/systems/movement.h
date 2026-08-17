#pragma once

#include "forge/ecs/components.h"
#include "forge/ecs/world.h"

namespace forge {

// Integrate Transform.position by Velocity.
// Also sync Sprite.rect position if entity has both Transform and Sprite.
class MovementSystem {
public:
    void update(ecs::World& world, float dt) {
        world.each<Velocity>([&](ecs::Entity e, Velocity& vel) {
            auto* tf = world.get<Transform>(e);
            if (tf) {
                tf->position += vel.value * dt;
            }
            auto* sprite = world.get<Sprite>(e);
            if (sprite && tf) {
                sprite->rect.x = tf->position.x;
                sprite->rect.y = tf->position.y;
            }
        });
    }
};

}  // namespace forge