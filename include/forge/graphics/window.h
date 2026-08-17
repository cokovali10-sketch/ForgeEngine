#pragma once

#include <cstdint>
#include <string>

struct SDL_Window;

namespace forge {

enum class WindowFlags {
    None = 0,
    Resizable = 1 << 0,
};

class Window {
public:
    Window(const std::string& title, int width, int height, WindowFlags flags = WindowFlags::None);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    int width() const;
    int height() const;
    bool should_close() const;
    void poll_events();

    SDL_Window* native() const { return window_; }
    void* native_handle() const;

private:
    SDL_Window* window_ = nullptr;
    int width_ = 0;
    int height_ = 0;
    bool closed_ = false;
};

}  // namespace forge