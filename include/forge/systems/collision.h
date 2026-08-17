#pragma once

#include "forge/ecs/components.h"
#include "forge/ecs/world.h"
#include "forge/graphics/sprite.h"
#include "forge/math/rect.h"
#include "forge/physics/aabb.h"

#include <vector>

namespace forge {

// Axis-separated AABB movement resolution ("sweep"):
// moves along X first, resolves contacts, then moves along Y and resolves.
// Gives stable landing on top of platforms and sliding along walls.
class CollisionSystem {
public:
    // Resolve `moveable` (a Sprite) against a set of static colliders.
    // Returns true if the entity touched ground this frame (for jump logic).
    bool update(ecs::World& world, ecs::Entity moveable, float dt) {
        auto* sprite = world.get<Sprite>(moveable);
        auto* tf = world.get<Transform>(moveable);
        auto* vel = world.get<Velocity>(moveable);
        if (!sprite || !tf) {
            return false;
        }

        std::vector<Rect> colliders;
        world.each<Sprite>([&](ecs::Entity e, Sprite& s) {
            if (e != moveable && s.solid) {
                colliders.push_back(s.rect);
            }
        });

        bool grounded = false;
        Rect player = sprite->rect;

        // --- Horizontal pass ---
        player.x += vel->value.x * dt;
        for (const Rect& c : colliders) {
            auto contact = physics::resolve_rect(player, c);
            if (contact.collided) {
                if (contact.normal.x != 0.0f) {
                    player.x += contact.normal.x * contact.penetration;
                    vel->value.x = 0.0f;
                }
            }
        }

        // --- Vertical pass ---
        player.y += vel->value.y * dt;
        for (const Rect& c : colliders) {
            auto contact = physics::resolve_rect(player, c);
            if (contact.collided) {
                if (contact.normal.y < 0.0f) {
                    player.y -= contact.penetration;   // ground pushes UP (y decreases)
                    vel->value.y = 0.0f;
                    grounded = true;
                } else if (contact.normal.y > 0.0f) {
                    player.y += contact.penetration;   // ceiling pushes DOWN (y increases)
                    vel->value.y = 0.0f;
                }
            }
        }

        sprite->rect = player;
        if (tf) {
            tf->position = {player.x, player.y};
        }
        return grounded;
    }
};

}  // namespace forge