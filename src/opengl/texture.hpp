#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include <exception>
#include <vector>
#include <string>

#include <cstdint>

#include <glad/glad.h>

namespace openglwrapper {
  class Texture {
  public:
    Texture();
    Texture(
      const std::vector<std::uint8_t> &data,
      int width, int height, int channels=3
    );
    ~Texture();

    // delete copy constructor/asignment
    Texture(Texture &other) = delete;
    Texture &operator=(Texture &other) = delete;

    // need to explicitly define move constructor
    Texture(Texture &&other);
    Texture &operator=(Texture &&other);

    ///////////////////////////////////////////////////////////////////////////
    void bind() const;
    void update(
      const std::vector<std::uint8_t> &data, int width, int height,
      int xoffset=0, int yoffset=0
    );
  private:
    GLuint id = 0;
    GLenum format;
  };
}

#endif // __TEXTURE_HPP__
