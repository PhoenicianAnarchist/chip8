#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include <filesystem>
#include <string>

#include <glm/glm.hpp>

#include <glad/glad.h>

namespace openglwrapper {
  class Shader {
  public:
    Shader();
    Shader(
      const std::filesystem::path &vshader_path,
      const std::filesystem::path &fshader_path
    );
    ~Shader();

    // delete copy constructor
    Shader(Shader &other) = delete;
    Shader &operator=(Shader &other) = delete;

    // need to explicitly define move constructor
    Shader(Shader &&other);
    Shader &operator=(Shader &&other);

    ///////////////////////////////////////////////////////////////////////////
    void use() const;

    // scalar uniforms
    void uniform4f(const std::string &name, const glm::vec4 &value) const;

    // matrix uniforms
    void uniformMatrix4fv(const std::string &name, const glm::mat4 &mat) const;

  private:
    GLuint id = 0;

    GLuint compileShader(GLenum shader_type, const GLchar *shader_string);
  };
}

#endif // __SHADER_HPP__
