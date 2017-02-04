#pragma once

#include "sge/core/array_buffer.h"
#include "sge/core/vertex_array.h"

#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>

namespace sge { namespace core {

class Mesh
{
public:
  Mesh(
    std::vector<unsigned short>* elements,
    std::vector<glm::vec3>* positions,
    std::vector<glm::vec3>* normals = nullptr,
    std::vector<glm::vec2>* texture_coordinates = nullptr,
    std::vector<glm::vec3>* tangents = nullptr,
    std::vector<glm::vec4>* colors = nullptr,
    GLenum render_mode = GL_TRIANGLES,
    GLenum render_method = GL_STATIC_DRAW);
  ~Mesh();

  virtual void render();
  glm::vec3 computeMinPosition() const;
  glm::vec3 computeMaxPosition() const;

protected:
  VertexArray _vao;
private:
  std::unique_ptr<ElementArrayBuffer> _element_buffer;

  // Mesh has ownership of this data!
  std::vector<unsigned short>* _elements;
  std::vector<glm::vec3>* _positions;
  std::vector<glm::vec3>* _normals;
  std::vector<glm::vec2>* _texture_coordinates;
  std::vector<glm::vec3>* _tangents;
  std::vector<glm::vec4>* _colors;
};

class CPUPointCloud : public Mesh
{
public:
  CPUPointCloud(std::vector<glm::vec3>* positions);

  void update(std::vector<glm::vec3>& positions);
  virtual void render();
private:
};

} }
