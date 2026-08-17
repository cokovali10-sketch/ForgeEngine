#pragma once

#include <array>

struct SDL_KeyboardEvent;

namespace forge {

enum class Key {
    W, A, S, D,
    Up, Down, Left, Right,
    Space, Enter, Escape, Tab, Shift,
    Count
};

class Input {
public:
    Input() = default;
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    void begin_frame();
    void handle_key(const SDL_KeyboardEvent& event);

    bool down(Key key) const { return current_[index(key)]; }
    bool pressed(Key key) const { return current_[index(key)] && !previous_[index(key)]; }
    bool released(Key key) const { return !current_[index(key)] && previous_[index(key)]; }

private:
    static std::size_t index(Key key) { return static_cast<std::size_t>(key); }

    std::array<bool, static_cast<std::size_t>(Key::Count)> current_{};
    std::array<bool, static_cast<std::size_t>(Key::Count)> previous_{};
};

}  // namespace forge