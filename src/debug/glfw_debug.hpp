#ifndef __GLFW_DEBUG__
#define __GLFW_DEBUG__

#include <GLFW/glfw3.h>

namespace debug {
  void enableDebugGLFW();
  void GLFWErrorCallback(int error, const char* description);
}

#endif // __GLFW_DEBUG__
