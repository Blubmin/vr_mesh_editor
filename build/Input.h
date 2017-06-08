#pragma once

#include <map>

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

class Input {
private:
    enum KeyState { KEY_UP, KEY_DOWN, KEY_PRESSED, KEY_RELEASED };
    enum ButtonState { BUTTON_UP, BUTTON_DOWN, BUTTON_PRESSED, BUTTON_RELEASED };

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static bool key_state_match(int key, KeyState state, int mods);
    static std::map<int, KeyState> _keys;
    static int _mods;

    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static glm::vec2 _cur_pos;
    static glm::vec2 _cur_delta;

    static void scroll_callback(GLFWwindow * window, double xoffset, double yoffset);
    static glm::vec2 _scroll;

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static bool button_state_match(int button, ButtonState state, int mods);
    static std::map<int, ButtonState> _buttons;

public:

    static void init(GLFWwindow* window);
    static void clear();
    static void poll_events();
    static bool key_down(int key, int mods = 0);
    static bool key_pressed(int key, int mods = 0);
    static bool key_released(int key, int mods = 0);
    static bool key_up(int key, int mods = 0);
    static bool key_pressed_down(int key, int mods = 0);
    static glm::vec2 cursor_pos();
    static glm::vec2 cursor_change();
    static glm::vec2 scroll();
    static bool button_down(int button, int mods = 0);
    static bool button_pressed(int button, int mods = 0);
    static bool button_up(int button, int mods = 0);
    static bool button_released(int button, int mods = 0);
    static bool button_pressed_down(int button, int mods = 0);
};

