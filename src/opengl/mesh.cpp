#include <utility>

#include "mesh.hpp"

openglwrapper::Mesh::Mesh() : vao(0), vbo(0), ebo(0) {}

openglwrapper::Mesh::Mesh(
  const std::vector<Vertex> &vertex_data,
  const std::vector<GLuint> &index_data
) {
  index_count = index_data.size();

  std::size_t position_count = vertex_data[0].position.length();
  std::size_t colour_count = vertex_data[0].colour.length();
  std::size_t coords_count = vertex_data[0].coords.length();
  std::size_t vertex_count = position_count + colour_count + coords_count;

  std::size_t position_size = sizeof(Vertex::position);
  std::size_t colour_size = sizeof(Vertex::colour);
  std::size_t coords_size = sizeof(Vertex::coords);
  std::size_t vertex_size = position_size + colour_size + coords_size;

  std::size_t colour_offset = position_size;
  std::size_t coords_offset = position_size + colour_size;
  std::size_t vertex_data_size = vertex_size * vertex_count;
  std::size_t index_data_size = sizeof(index_data[0]) * index_count;

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(
    GL_ARRAY_BUFFER, vertex_data_size, vertex_data.data(), GL_STATIC_DRAW
  );

  glVertexAttribPointer(
    0, position_count, GL_FLOAT, GL_FALSE, vertex_size, (void*)0
  );
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    1, colour_count, GL_FLOAT, GL_FALSE, vertex_size, (void*)(colour_offset)
  );
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
    2, coords_count, GL_FLOAT, GL_FALSE, vertex_size, (void*)(coords_offset)
  );
  glEnableVertexAttribArray(2);

  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, index_data_size, index_data.data(), GL_STATIC_DRAW
  );

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

openglwrapper::Mesh::~Mesh() {
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
}

openglwrapper::Mesh::Mesh(Mesh &&other)
: vao(std::exchange(other.vao, 0)),
  vbo(std::exchange(other.vbo, 0)),
  ebo(std::exchange(other.ebo, 0))
{}

openglwrapper::Mesh &openglwrapper::Mesh::operator=(Mesh &&other) {
  vao = std::exchange(other.vao, 0);
  vbo = std::exchange(other.vbo, 0);
  ebo = std::exchange(other.ebo, 0);

  return *this;
}

///////////////////////////////////////////////////////////////////////////////
void openglwrapper::Mesh::draw() const {
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
}
