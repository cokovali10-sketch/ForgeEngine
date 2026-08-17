#pragma once

#include "forge/ecs/components.h"
#include "forge/ecs/world.h"
#include "forge/graphics/renderer.h"
#include "forge/graphics/sprite.h"

namespace forge {

// Draw all Sprite entities.
class RenderSystem {
public:
    void update(ecs::World& world, Renderer& renderer) {
        world.each<Sprite>([&](ecs::Entity e, Sprite& sprite) {
            renderer.draw_rect(sprite.rect, sprite.color);
        });
    }
};

}  // namespace forge