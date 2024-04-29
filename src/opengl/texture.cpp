#include <utility>

#include "../util/io.hpp"
#include "exceptions.hpp"
#include "texture.hpp"

openglwrapper::Texture::Texture() : id(0) {}

openglwrapper::Texture::Texture(
  const std::vector<std::uint8_t> &data, int width, int height, int channels
) : id(0) {
  switch (channels) {
    case 1: format = GL_RED;  break;
    case 3: format = GL_RGB;  break;
    case 4: format = GL_RGBA; break;
  }

  GLenum internalFormat;
  switch (channels) {
    case 1: internalFormat = GL_RED;  break;
    case 3: internalFormat = GL_RGBA; break;
    case 4: internalFormat = GL_RGBA; break;
  }

  glGenTextures(1, &id);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // glTexImage2D(
  //  GLenum target​, GLint level​, GLint internalFormat​,
  //  GLsizei width​, GLsizei height​,
  //  GLint border​, GLenum format​, GLenum type​, const GLvoid * data​
  // )
  glTexImage2D(
    GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format,
    GL_UNSIGNED_BYTE, data.data()
  );

  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
}

openglwrapper::Texture::~Texture() {
  glDeleteTextures(1, &id);
}

openglwrapper::Texture::Texture(Texture &&other)
: id(std::exchange(other.id, 0)) {}

openglwrapper::Texture &openglwrapper::Texture::operator=(Texture &&other) {
  id = std::exchange(other.id, 0);

  return *this;
}

///////////////////////////////////////////////////////////////////////////////

void openglwrapper::Texture::bind() const {
  glBindTexture(GL_TEXTURE_2D, id);
}

void openglwrapper::Texture::update(
  const std::vector<std::uint8_t> &data, int width, int height,
  int xoffset, int yoffset
) {
  glBindTexture(GL_TEXTURE_2D, id);

  glTexSubImage2D(
    GL_TEXTURE_2D, 0, xoffset, yoffset, width, height, format,
    GL_UNSIGNED_BYTE, data.data()
  );
}
