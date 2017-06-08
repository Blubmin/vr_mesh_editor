#include "Input.h"

using namespace glm;
using namespace std;

std::map<int, Input::KeyState> Input::_keys;
std::map<int, Input::ButtonState> Input::_buttons;
int Input::_mods;
glm::vec2 Input::_cur_pos;
glm::vec2 Input::_cur_delta;
glm::vec2 Input::_scroll;

void Input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    _mods = mods;

    KeyState prev = _keys[key];

    if (action == GLFW_PRESS) {
        if (prev == KEY_PRESSED) {
            _keys[key] = KEY_DOWN;
        }
        else {
            _keys[key] = KEY_PRESSED;
        }
    }
    else if (action == GLFW_REPEAT) {
        _keys[key] = KEY_DOWN;
    }
    else if (action == GLFW_RELEASE) {
        if (prev == KEY_RELEASED || prev == KEY_UP) {
            _keys[key] = KEY_UP;
        }
        else {
            _keys[key] = KEY_RELEASED;
        }
    }
}

void Input::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    vec2 new_pos = vec2(xpos, ypos);
    _cur_delta = new_pos - _cur_pos;
    _cur_pos = new_pos;
}

void Input::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    static vec2 scroll_pos;

    vec2 new_scroll = vec2(xoffset, yoffset);

    _scroll = new_scroll;

    scroll_pos = new_scroll;
}

void Input::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    _mods = mods;

    ButtonState prev = _buttons[button];

    if (action == GLFW_PRESS) {
        if (prev == BUTTON_PRESSED || prev == BUTTON_DOWN) {
            _buttons[button] = BUTTON_DOWN;
        }
        else {
            _buttons[button] = BUTTON_PRESSED;
        }
    }
    else if (action == GLFW_RELEASE) {
        if (prev == BUTTON_RELEASED || prev == BUTTON_UP) {
            _buttons[button] = BUTTON_UP;
        }
        else {
            _buttons[button] = BUTTON_RELEASED;
        }
    }
}

void Input::init(GLFWwindow* window) {
    for (int i = 0; i < GLFW_KEY_LAST; i++) _keys[i] = KEY_UP;
    for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++) _buttons[i] = BUTTON_UP;
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
}

void Input::poll_events() {
    for (int i = 0; i < GLFW_KEY_LAST; i++) {
        if (_keys[i] == KEY_PRESSED) {
            _keys[i] == KEY_DOWN;
        }
        if (_keys[i] == KEY_RELEASED) {
            _keys[i] == KEY_UP;
        }
    }

    for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++) {
        if (_buttons[i] == BUTTON_PRESSED) {
            _buttons[i] == BUTTON_DOWN;
        }
        if (_buttons[i] == BUTTON_RELEASED) {
            _buttons[i] == BUTTON_UP;
        }
    }
}

void Input::clear() {
    _cur_delta = vec2();
    _scroll = vec2();
}

bool Input::key_state_match(int key, KeyState state, int mods) {
    return _keys[key] == state && (_mods & mods) == mods;
}

bool Input::key_down(int key, int mods) {
    return key_state_match(key, KEY_DOWN, mods);
}

bool Input::key_pressed(int key, int mods) {
    return key_state_match(key, KEY_PRESSED, mods);
}

bool Input::key_released(int key, int mods) {
    return key_state_match(key, KEY_RELEASED, mods);
}

bool Input::key_up(int key, int mods) {
    return key_state_match(key, KEY_UP, mods);
}

bool Input::key_pressed_down(int key, int mods) {
    return key_down(key, mods) || key_pressed(key, mods);
}

glm::vec2 Input::cursor_pos() {
    return _cur_pos;
}

glm::vec2 Input::cursor_change() {
    return _cur_delta;
}

glm::vec2 Input::scroll() {
    return _scroll;
}

bool Input::button_state_match(int button, ButtonState state, int mods) {
    return _buttons[button] == state && (_mods & mods) == mods;
}

bool Input::button_down(int button, int mods) {
    return button_state_match(button, BUTTON_DOWN, mods);
}

bool Input::button_pressed(int button, int mods) {
    return button_state_match(button, BUTTON_PRESSED, mods);
}

bool Input::button_released(int button, int mods) {
    return button_state_match(button, BUTTON_RELEASED, mods);
}

bool Input::button_up(int button, int mods) {
    return button_state_match(button, BUTTON_UP, mods);
}

bool Input::button_pressed_down(int button, int mods) {
    return button_pressed(button, mods) || button_down(button, mods);
}