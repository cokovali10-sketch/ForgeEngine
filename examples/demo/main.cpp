#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <iostream>
#include <vector>

#include "forge/assets/generated_palette.h"
#include "forge/ecs/components.h"
#include "forge/ecs/world.h"
#include "forge/graphics/renderer.h"
#include "forge/graphics/sprite.h"
#include "forge/graphics/window.h"
#include "forge/input/input.h"
#include "forge/math/rect.h"
#include "forge/physics/aabb.h"
#include "forge/systems/movement.h"
#include "forge/systems/render.h"

namespace {

constexpr int kWidth = 960;
constexpr int kHeight = 540;
constexpr float kGravity = 980.0f;
constexpr float kJump = -380.0f;
constexpr float kSpeed = 240.0f;

const char* kTitle = "ForgeEngine Demo — Platformer";

}  // namespace

int main() {
    try {
        forge::Window window(kTitle, kWidth, kHeight, forge::WindowFlags::Resizable);
        forge::Renderer renderer(window);
        forge::Input input;

        forge::ecs::World world;

        // Player.
        auto player = world.create();
        auto& player_sprite = world.add<forge::Sprite>(
            player, forge::Rect(320.0f, 200.0f, 32.0f, 48.0f), forge::assets::sky);
        world.add<forge::Velocity>(player, forge::Vec2{0.0f, 0.0f});
        world.add<forge::Transform>(player);

        // Ground platform.
        auto ground = world.create();
        world.add<forge::Sprite>(
            ground, forge::Rect(0.0f, kHeight - 80.0f, kWidth, 80.0f), forge::assets::emerald);

        // Floating platforms.
        std::vector<forge::ecs::Entity> platforms;
        for (auto [x, y, w] : {std::tuple(120.0f, 340.0f, 160.0f),
                               std::tuple(400.0f, 280.0f, 160.0f),
                               std::tuple(680.0f, 340.0f, 160.0f)}) {
            auto p = world.create();
            world.add<forge::Sprite>(p, forge::Rect(x, y, w, 24.0f), forge::assets::gold);
            platforms.push_back(p);
        }

        bool running = true;
        uint64_t last = SDL_GetPerformanceCounter();

        while (running) {
            window.poll_events();
            if (window.should_close()) {
                running = false;
            }

            uint64_t now = SDL_GetPerformanceCounter();
            float dt = static_cast<float>(now - last) / SDL_GetPerformanceFrequency();
            last = now;
            if (dt > 0.05f) {
                dt = 0.05f;
            }

            input.begin_frame();
            // Poll SDL events into Input (KEYDOWN/KEYUP).
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                    input.handle_key(event.key);
                }
            }

            auto* vel = world.get<forge::Velocity>(player);
            auto* tf = world.get<forge::Transform>(player);

            if (vel && tf) {
                float move = 0.0f;
                if (input.down(forge::Key::A) || input.down(forge::Key::Left)) move -= 1.0f;
                if (input.down(forge::Key::D) || input.down(forge::Key::Right)) move += 1.0f;

                vel->value.x = move * kSpeed;
                vel->value.y += kGravity * dt;

                if (input.pressed(forge::Key::Space)) {
                    vel->value.y = kJump;
                }
            }

            // Integrate movement.
            forge::MovementSystem movement;
            movement.update(world, dt);

            // AABB collision vs platforms (sweep by axis).
            if (tf) {
                auto* sprite = world.get<forge::Sprite>(player);
                forge::Rect player_rect = sprite->rect;
                player_rect.y = tf->position.y;

                for (auto& p : platforms) {
                    auto* ps = world.get<forge::Sprite>(p);
                    if (!ps) continue;
                    auto contact = forge::physics::resolve_rect(player_rect, ps->rect);
                    if (contact.collided) {
                        if (contact.normal.y < 0.0f) {
                            player_rect.y += contact.penetration;
                            tf->position.y = player_rect.y;
                            if (vel) vel->value.y = 0.0f;
                        }
                    }
                }
                sprite->rect.x = tf->position.x;
                sprite->rect.y = tf->position.y;
            }

            renderer.begin_frame(forge::assets::midnight);
            forge::RenderSystem render;
            render.update(world, renderer);
            renderer.end_frame();
        }
    } catch (const std::exception& e) {
        std::cerr << "ForgeEngine demo crashed: " << e.what() << "\n";
        return 1;
    }
    return 0;
}