#include <utility>
#include <vector>

#include <glm/gtc/type_ptr.hpp>

#include "../util/io.hpp"
#include "exceptions.hpp"
#include "shader.hpp"

openglwrapper::Shader::Shader() : id(0) {}

openglwrapper::Shader::Shader(
  const std::filesystem::path &vshader_path,
  const std::filesystem::path &fshader_path
) {
  // Vertex Shader
  std::string vshader_source = utils::read_file(vshader_path);
  GLuint vshader = compileShader(GL_VERTEX_SHADER, vshader_source.c_str());

  // Fragment Shader
  std::string fshader_source = utils::read_file(fshader_path);
  GLuint fshader = compileShader(GL_FRAGMENT_SHADER, fshader_source.c_str());

  // Shader Program
  id = glCreateProgram();
  glAttachShader(id, vshader);
  glAttachShader(id, fshader);
  glLinkProgram(id);

  GLint is_linked;
  glGetProgramiv(id, GL_LINK_STATUS, &is_linked);
  if (is_linked == GL_FALSE) {
    GLint max_length;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &max_length);

    std::vector<GLchar> info_log(max_length);
    glGetProgramInfoLog(id, max_length, &max_length, &info_log[0]);

    glDeleteProgram(id);

    std::stringstream ss;
    ss << "program linking error\n";
    ss << "  vertex: " << vshader << "\n";
    ss << "  fragment: " << fshader << "\n";
    ss << "  program: " << id << "\n";
    ss << info_log.data();
    throw shader_error(ss.str());
  }

  // free memory
  glDetachShader(id, vshader);
  glDetachShader(id, fshader);
  glDeleteShader(vshader);
  glDeleteShader(fshader);
}

openglwrapper::Shader::~Shader() {
  glDeleteProgram(id);
}

openglwrapper::Shader::Shader(Shader &&other)
: id(std::exchange(other.id, 0)) {}

openglwrapper::Shader &openglwrapper::Shader::operator=(Shader &&other) {
  id = std::exchange(other.id, 0);

  return *this;
}

///////////////////////////////////////////////////////////////////////////////
void openglwrapper::Shader::use() const {
  glUseProgram(id);
}

// scalar uniforms
void openglwrapper::Shader::uniform4f(
  const std::string &name, const glm::vec4 &value
) const {
  GLint location = glGetUniformLocation(id, name.c_str());
  glUniform4f(location, value.x, value.y, value.z, value.w);
}

// matrix uniforms
void openglwrapper::Shader::uniformMatrix4fv(
  const std::string &name, const glm::mat4 &mat
) const {
  GLint location = glGetUniformLocation(id, name.c_str());
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

///////////////////////////////////////////////////////////////////////////////
GLuint openglwrapper::Shader::compileShader(
  GLenum shader_type, const GLchar *shader_string
) {
  // `glShaderSource` takes an array of <const char *>
  //    shader_id
  //    count of strings
  //    array of strings
  //    array of string lengths (if non-null terminated strings are passed)

  GLuint shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &shader_string, nullptr);
  glCompileShader(shader);

  GLint is_compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
  if (is_compiled == GL_FALSE) {
    GLint max_length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

    std::vector<GLchar> info_log(max_length);
    glGetShaderInfoLog(shader, max_length, &max_length, &info_log[0]);

    glDeleteShader(shader);

    std::stringstream ss;
    ss << "---" << std::endl;
    switch (shader_type) {
      case GL_VERTEX_SHADER: ss << "Vertex "; break;
      case GL_FRAGMENT_SHADER: ss << "Fragment "; break;
    }
    ss << "shader compilation error\n" << info_log.data();

    throw shader_error(ss.str());
  }

  return shader;
}
