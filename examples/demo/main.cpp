#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

#include "forge/assets/generated_palette.h"
#include "forge/ecs/components.h"
#include "forge/ecs/world.h"
#include "forge/graphics/renderer.h"
#include "forge/graphics/sprite.h"
#include "forge/graphics/window.h"
#include "forge/input/input.h"
#include "forge/math/rect.h"
#include "forge/systems/collision.h"
#include "forge/systems/movement.h"
#include "forge/systems/render.h"

namespace {

constexpr int kWidth = 960;
constexpr int kHeight = 540;
constexpr float kGravity = 1100.0f;
constexpr float kJump = -480.0f;
constexpr float kSpeed = 260.0f;

const char* kTitle = "ForgeEngine Demo";

// Deterministic "stars" for a pseudo-random-looking backdrop.
struct Star {
    forge::Vec2 pos;
    forge::Color color;
    float twinkle;
};

std::vector<Star> make_stars() {
    std::vector<Star> stars;
    stars.reserve(60);
    unsigned seed = 2026u;
    auto rnd = [&seed]() {
        seed = seed * 1664525u + 1013904223u;
        return static_cast<float>(seed % 10000u) / 10000.0f;
    };
    for (int i = 0; i < 60; ++i) {
        Star s;
        s.pos = {rnd() * kWidth, rnd() * (kHeight - 120.0f)};
        float b = 0.35f + rnd() * 0.65f;
        s.color = forge::Color(b, b, b, 0.9f);
        s.twinkle = 0.5f + rnd();
        stars.push_back(s);
    }
    return stars;
}

}  // namespace

int main(int argc, char* argv[]) {
    std::string screenshot_path;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--screenshot" && i + 1 < argc) {
            screenshot_path = argv[++i];
        }
    }

    try {
        forge::Window window(kTitle, kWidth, kHeight, forge::WindowFlags::Resizable);
        forge::Renderer renderer(window);
        forge::Input input;

        forge::ecs::World world;

        // --- Player ---
        auto player = world.create();
        world.add<forge::Transform>(player, forge::Vec2{320.0f, 180.0f});
        world.add<forge::Velocity>(player, forge::Vec2{0.0f, 0.0f});
        world.add<forge::Sprite>(
            player, forge::Rect(320.0f, 180.0f, 34.0f, 50.0f), forge::assets::sky, true);

        // --- Ground ---
        auto ground = world.create();
        world.add<forge::Sprite>(
            ground, forge::Rect(0.0f, kHeight - 80.0f, kWidth, 80.0f), forge::assets::emerald, true);

        // --- Platforms ---
        std::vector<forge::ecs::Entity> platforms;
        for (auto [x, y, w] : {std::tuple(120.0f, 360.0f, 170.0f),
                               std::tuple(420.0f, 300.0f, 150.0f),
                               std::tuple(680.0f, 360.0f, 160.0f),
                               std::tuple(520.0f, 210.0f, 120.0f)}) {
            auto p = world.create();
            world.add<forge::Sprite>(p, forge::Rect(x, y, w, 22.0f), forge::assets::gold, true);
            platforms.push_back(p);
        }

        // --- Decorative sprites (non-solid) ---
        auto block = world.create();
        world.add<forge::Sprite>(block, forge::Rect(50.0f, 120.0f, 40.0f, 40.0f),
                                 forge::assets::crimson, false);
        auto block2 = world.create();
        world.add<forge::Sprite>(block2, forge::Rect(860.0f, 150.0f, 32.0f, 32.0f),
                                 forge::assets::crimson, false);

        std::vector<Star> stars = make_stars();

        // --- Loop state ---
        bool running = true;
        uint64_t last = SDL_GetPerformanceCounter();
        float fps = 0.0f;
        int frames = 0;
        float fps_timer = 0.0f;
        bool grounded = false;
        int captured = -1;  // -1 = disabled; >=0 = frames remaining before shot

        if (!screenshot_path.empty()) {
            captured = 90;  // let the player fall to the ground, then capture
        }

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

            // FPS counter (1 Hz refresh).
            ++frames;
            fps_timer += dt;
            if (fps_timer >= 0.5f) {
                fps = static_cast<float>(frames) / fps_timer;
                frames = 0;
                fps_timer = 0.0f;
                window.set_title(kTitle + std::string("  —  ") +
                                 std::to_string(static_cast<int>(fps)) + " FPS");
            }

            input.begin_frame();
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                    input.handle_key(event.key);
                }
            }

            auto* vel = world.get<forge::Velocity>(player);
            if (vel) {
                float move = 0.0f;
                if (input.down(forge::Key::A) || input.down(forge::Key::Left)) move -= 1.0f;
                if (input.down(forge::Key::D) || input.down(forge::Key::Right)) move += 1.0f;
                vel->value.x = move * kSpeed;
                vel->value.y += kGravity * dt;

                if (input.pressed(forge::Key::Space) && grounded) {
                    vel->value.y = kJump;
                    grounded = false;
                }
            }

            forge::MovementSystem movement;
            movement.update(world, dt);

            forge::CollisionSystem collision;
            grounded = collision.update(world, player, dt);

            // Horizontal boundary - disabled for now, player stays centered
            // if (auto* sprite = world.get<forge::Sprite>(player)) {
            //     if (sprite->rect.x < 0.0f) sprite->rect.x = 0.0f;
            //     if (sprite->rect.right() > kWidth) sprite->rect.x = kWidth - sprite->rect.w;
            //     if (auto* tf = world.get<forge::Transform>(player)) {
            //         tf->position = {sprite->rect.x, sprite->rect.y};
            //     }
            // }

            // --- Render ---
            renderer.begin_frame(forge::assets::midnight);

            // Starfield.
            float t = static_cast<float>(SDL_GetTicks()) / 1000.0f;
            for (const Star& s : stars) {
                float b = s.color.r * (0.7f + 0.3f * std::sin(t * s.twinkle));
                renderer.draw_rect(forge::Rect(s.pos.x, s.pos.y, 2.0f, 2.0f),
                                   forge::Color(b, b, b));
            }

            forge::RenderSystem render;
            render.update(world, renderer);

            renderer.end_frame();

            // --- Screenshot capture ---
            if (captured >= 0) {
                --captured;
                if (captured == 0) {
                    renderer.save_screenshot(screenshot_path);
                    std::cout << "Saved screenshot: " << screenshot_path << "\n";
                    running = false;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "ForgeEngine demo crashed: " << e.what() << "\n";
        return 1;
    }
    return 0;
}