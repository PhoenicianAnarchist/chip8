#ifndef __INPUT_HPP__
#define __INPUT_HPP__

#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace openglwrapper {
  struct KeyState {
    bool is_pressed;
    bool is_released;
    bool is_handled;
  };

  extern std::map<int /* key #define  */ , KeyState> key_states;

  void key_callback(
    GLFWwindow *window, int key, int scancode, int action, int mods
  );
}

#endif // __INPUT_HPP__
