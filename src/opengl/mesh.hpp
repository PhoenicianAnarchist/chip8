#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <vector>

#include <glm/glm.hpp>

#include <glad/gl.h>

namespace openglwrapper {
  struct Vertex {
    glm::vec2 position;
    glm::vec3 colour;
    glm::vec2 coords;
  };

  class Mesh {
  public:
    Mesh();
    Mesh(
      const std::vector<Vertex> &vertex_data,
      const std::vector<GLuint> &index_data
    );
    ~Mesh();

    // delete copy constructor
    Mesh(Mesh &other) = delete;
    Mesh &operator=(Mesh &other) = delete;

    // need to explicitly define move constructor
    Mesh(Mesh &&other);
    Mesh &operator=(Mesh &&other);

    /////////////////////////////////////////////////////////////////////////////
    void draw() const;
  private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    GLuint index_count;
  };
}

#endif // __MESH_HPP__
