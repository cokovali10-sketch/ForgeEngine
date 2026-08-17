#include "forge/input/input.h"

#include <SDL.h>

namespace forge {

namespace {
Key map_scancode(SDL_Scancode sc) {
    switch (sc) {
        case SDL_SCANCODE_W: return Key::W;
        case SDL_SCANCODE_A: return Key::A;
        case SDL_SCANCODE_S: return Key::S;
        case SDL_SCANCODE_D: return Key::D;
        case SDL_SCANCODE_UP: return Key::Up;
        case SDL_SCANCODE_DOWN: return Key::Down;
        case SDL_SCANCODE_LEFT: return Key::Left;
        case SDL_SCANCODE_RIGHT: return Key::Right;
        case SDL_SCANCODE_SPACE: return Key::Space;
        case SDL_SCANCODE_RETURN: return Key::Enter;
        case SDL_SCANCODE_ESCAPE: return Key::Escape;
        case SDL_SCANCODE_TAB: return Key::Tab;
        case SDL_SCANCODE_LSHIFT:
        case SDL_SCANCODE_RSHIFT: return Key::Shift;
        default: return Key::Count;
    }
}
}  // namespace

void Input::begin_frame() {
    previous_ = current_;
}

void Input::handle_key(const SDL_KeyboardEvent& event) {
    Key key = map_scancode(event.keysym.scancode);
    if (key == Key::Count) {
        return;
    }
    current_[index(key)] = event.type == SDL_KEYDOWN;
}

}  // namespace forge