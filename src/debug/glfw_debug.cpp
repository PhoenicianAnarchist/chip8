#include <iostream>

#include "glfw_debug.hpp"

void debug::enableDebugGLFW() {
  glfwSetErrorCallback(GLFWErrorCallback);
}

void debug::GLFWErrorCallback(int error, const char* description) {
  std::cout << error << " : " << description << std::endl;
}
