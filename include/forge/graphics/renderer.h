#pragma once

#include <string>

#include "forge/math/rect.h"

struct SDL_Renderer;

namespace forge {

class Window;

class Renderer {
public:
    explicit Renderer(Window& window);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void begin_frame(const Color& clear);
    void end_frame();

    void draw_rect(const Rect& rect, const Color& color);
    void draw_rect_outline(const Rect& rect, const Color& color, float thickness = 1.0f);
    void draw_line(float x1, float y1, float x2, float y2, const Color& color);

    // Capture the current framebuffer to a BMP file.
    bool save_screenshot(const std::string& path);

    int width() const;
    int height() const;

private:
    SDL_Renderer* renderer_ = nullptr;
    int width_ = 0;
    int height_ = 0;
};

}  // namespace forge