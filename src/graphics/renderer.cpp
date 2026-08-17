#include "forge/graphics/renderer.h"

#include <SDL.h>
#include <stdexcept>

#include "forge/graphics/window.h"

namespace forge {

namespace {
SDL_Color to_sdl(const Color& c) {
    return {static_cast<uint8_t>(c.r * 255.0f),
            static_cast<uint8_t>(c.g * 255.0f),
            static_cast<uint8_t>(c.b * 255.0f),
            static_cast<uint8_t>(c.a * 255.0f)};
}
}  // namespace

Renderer::Renderer(Window& window) {
    renderer_ = SDL_CreateRenderer(window.native(), -1,
                                   SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());
    }
    SDL_GetRendererOutputSize(renderer_, &width_, &height_);
}

Renderer::~Renderer() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
    }
}

void Renderer::begin_frame(const Color& clear) {
    SDL_Color c = to_sdl(clear);
    SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, c.a);
    SDL_RenderClear(renderer_);
}

void Renderer::end_frame() {
    SDL_RenderPresent(renderer_);
}

void Renderer::draw_rect(const Rect& rect, const Color& color) {
    SDL_Color c = to_sdl(color);
    SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, c.a);
    SDL_Rect r{static_cast<int>(rect.x), static_cast<int>(rect.y),
               static_cast<int>(rect.w), static_cast<int>(rect.h)};
    SDL_RenderFillRect(renderer_, &r);
}

void Renderer::draw_rect_outline(const Rect& rect, const Color& color, float thickness) {
    SDL_Color c = to_sdl(color);
    SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, c.a);
    SDL_Rect r{static_cast<int>(rect.x), static_cast<int>(rect.y),
               static_cast<int>(rect.w), static_cast<int>(rect.h)};
    for (int i = 0; i < static_cast<int>(thickness); ++i) {
        SDL_RenderDrawRect(renderer_, &r);
        r.x += 1;
        r.y += 1;
        r.w -= 2;
        r.h -= 2;
    }
}

void Renderer::draw_line(float x1, float y1, float x2, float y2, const Color& color) {
    SDL_Color c = to_sdl(color);
    SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, c.a);
    SDL_RenderDrawLine(renderer_, static_cast<int>(x1), static_cast<int>(y1),
                       static_cast<int>(x2), static_cast<int>(y2));
}

bool Renderer::save_screenshot(const std::string& path) {
    int w = 0, h = 0;
    SDL_GetRendererOutputSize(renderer_, &w, &h);
    SDL_Surface* surface = SDL_CreateRGBSurface(0, w, h, 32, 0x00FF0000, 0x0000FF00,
                                                0x000000FF, 0xFF000000);
    if (!surface) {
        return false;
    }
    if (SDL_RenderReadPixels(renderer_, nullptr, surface->format->format,
                             surface->pixels, surface->pitch) != 0) {
        SDL_FreeSurface(surface);
        return false;
    }
    bool ok = SDL_SaveBMP(surface, path.c_str()) == 0;
    SDL_FreeSurface(surface);
    return ok;
}

int Renderer::width() const { return width_; }
int Renderer::height() const { return height_; }

}  // namespace forge