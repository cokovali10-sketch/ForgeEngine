#include "forge/graphics/window.h"

#include <SDL.h>
#include <stdexcept>

namespace forge {

Window::Window(const std::string& title, int width, int height, WindowFlags flags)
    : width_(width), height_(height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
    }

    uint32_t sdl_flags = SDL_WINDOW_SHOWN;
    if (static_cast<int>(flags) & static_cast<int>(WindowFlags::Resizable)) {
        sdl_flags |= SDL_WINDOW_RESIZABLE;
    }

    window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               width, height, sdl_flags);
    if (!window_) {
        SDL_Quit();
        throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
    }
}

Window::~Window() {
    if (window_) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

int Window::width() const { return width_; }
int Window::height() const { return height_; }

bool Window::should_close() const { return closed_; }

void Window::poll_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            closed_ = true;
        } else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            width_ = event.window.data1;
            height_ = event.window.data2;
        }
    }
}

void* Window::native_handle() const { return window_; }

}  // namespace forge