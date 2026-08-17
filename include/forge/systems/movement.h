#pragma once

#include "forge/ecs/components.h"
#include "forge/ecs/world.h"

namespace forge {

// Integrate Transform.position by Velocity.
class MovementSystem {
public:
    void update(ecs::World& world, float dt) {
        world.each<Velocity>([&](ecs::Entity e, Velocity& vel) {
            if (auto* tf = world.get<Transform>(e)) {
                tf->position += vel.value * dt;
            }
        });
    }
};

}  // namespace forge