#ifndef __GL_DEBUG__
#define __GL_DEBUG__

#include <glad/glad.h>

namespace debug {
  bool enableDebug();

  void APIENTRY glDebugMessage(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar *message, const void *userParam
  );
}

#endif // __GL_DEBUG__
