#include "input.hpp"


std::map<int, openglwrapper::KeyState> openglwrapper::key_states;

void openglwrapper::key_callback(
  GLFWwindow *window, int key, int scancode, int action, int mods
) {
  if (action == GLFW_PRESS) {
    key_states[key].is_pressed  = true;
    key_states[key].is_released = false;
    key_states[key].is_handled  = false;
  } else if (action == GLFW_RELEASE) {
    key_states[key].is_pressed  = false;
    key_states[key].is_released = true;
    key_states[key].is_handled  = false;
  }
}
