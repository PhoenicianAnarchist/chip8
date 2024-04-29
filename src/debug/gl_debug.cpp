#include <iostream>

#include "gl_debug.hpp"

bool debug::enableDebug() {
  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    // ensure callback is called in the same thread and scope
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugMessage, nullptr);
    // disable message filtering
    glDebugMessageControl(
      GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE
    );

    return true;
  }

  return false;
}

void APIENTRY debug::glDebugMessage(
  GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/,
  const GLchar *message, const void */*userParam*/
) {
  std::cout << "---" << std::endl;
  std::cout << "DEBUG MESSAGE (" << id << "): " <<  message << std::endl;

  std::cout << "Source  : ";
  switch (source) {
    case GL_DEBUG_SOURCE_API:
      std::cout << "api";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      std::cout << "window system";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      std::cout << "shader compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      std::cout << "third party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      std::cout << "application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      std::cout << "other";
      break;
  }

  std::cout << std::endl;

  std::cout << "Type    : ";
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      std::cout << "error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      std::cout << "deprecated behaviour";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      std::cout << "undefined behaviour";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      std::cout << "portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      std::cout << "performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      std::cout << "marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      std::cout << "push group";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      std::cout << "pop group";
      break;
    case GL_DEBUG_TYPE_OTHER:
      std::cout << "other";
      break;
  }

  std::cout << std::endl;

  std::cout << "Severity : ";
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      std::cout << "high";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      std::cout << "medium";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      std::cout << "low";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      std::cout << "notification";
      break;
  }

  std::cout << std::endl;
}
