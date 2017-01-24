#include "sge/core/new_mesh.h"

namespace sge { namespace core {

NewMesh::NewMesh(
  std::vector<unsigned short>* elements,
  std::vector<glm::vec3>* positions,
  std::vector<glm::vec3>* normals,
  std::vector<glm::vec2>* texture_coordinates,
  std::vector<glm::vec3>* tangents,
  std::vector<glm::vec4>* colors,
  GLenum render_mode,
  GLenum render_method) :
  _elements(elements),
  _positions(positions),
  _normals(normals),
  _texture_coordinates(texture_coordinates),
  _tangents(tangents),
  _colors(colors)
{
  assert(positions);
  if (_elements)
  {
    ArrayBuffer::InitData init_data =
      {&_elements->at(0), sizeof(unsigned short) * _elements->size(),
      static_cast<GLuint>(_elements->size()), GL_UNSIGNED_SHORT,
      GL_ELEMENT_ARRAY_BUFFER, render_method, render_mode};
    _element_buffer = std::make_unique<ElementArrayBuffer>(init_data);
  }
  if (_positions)
  {
    ArrayBuffer::InitData init_data =
      {&_positions->at(0), sizeof(glm::vec3) * _positions->size(),
      static_cast<GLuint>(_positions->size()), GL_FLOAT, GL_ARRAY_BUFFER,
      render_method, render_mode};
    _vao.addBuffer(init_data, 0, 3);
  }
  if (_normals)
  {
    ArrayBuffer::InitData init_data =
      {&_normals->at(0), sizeof(glm::vec3) * _normals->size(),
      static_cast<GLuint>(_normals->size()), GL_FLOAT, GL_ARRAY_BUFFER,
      render_method, render_mode};
    _vao.addBuffer(init_data, 1, 3);
  }
  if (_texture_coordinates)
  {
    ArrayBuffer::InitData init_data =
      {&_texture_coordinates->at(0), sizeof(glm::vec2) *
      _texture_coordinates->size(),
      static_cast<GLuint>(_texture_coordinates->size()), GL_FLOAT,
      GL_ARRAY_BUFFER, render_method, render_mode};
    _vao.addBuffer(init_data, 2, 2);
  }
  if (_tangents)
  {
    ArrayBuffer::InitData init_data =
      {&_tangents->at(0), sizeof(glm::vec3) * _tangents->size(),
      static_cast<GLuint>(_tangents->size()), GL_FLOAT, GL_ARRAY_BUFFER,
      render_method, render_mode};
    _vao.addBuffer(init_data, 3, 3);
  }
  if (_colors)
  {
    ArrayBuffer::InitData init_data =
      {&_colors->at(0), sizeof(glm::vec4) * _colors->size(),
      static_cast<GLuint>(_colors->size()), GL_FLOAT, GL_ARRAY_BUFFER,
      render_method, render_mode};
    _vao.addBuffer(init_data, 4, 4);
  }
}

NewMesh::~NewMesh()
{
  if (_elements)
    delete _elements;
  if (_positions)
    delete _positions;
  if (_normals)
    delete _normals;
  if (_texture_coordinates)
    delete _texture_coordinates;
  if (_tangents)
    delete _tangents;
  if (_colors)
    delete _colors;
}

void NewMesh::render()
{
  _element_buffer->bind();
  _vao.bind();
  _vao.enableAttribArrays();
  _element_buffer->render();
  _vao.disableAttribArrays();
}

glm::vec3 NewMesh::computeMinPosition() const
{
  glm::vec3 max = _positions->at(0);
  for (int i = 1; i < _positions->size(); i++)
    max = glm::max(max, _positions->at(i));
  return max;
}

glm::vec3 NewMesh::computeMaxPosition() const
{
  glm::vec3 min = _positions->at(0);
  for (int i = 1; i < _positions->size(); i++)
    min = glm::min(min, _positions->at(i));
  return min;
}


NewCPUPointCloud::NewCPUPointCloud(std::vector<glm::vec3>* positions) :
  NewMesh(nullptr, positions, nullptr, nullptr, nullptr, nullptr,
    GL_POINTS, GL_DYNAMIC_DRAW)
{

}

void NewCPUPointCloud::render()
{
  _vao.bind();
  _vao.enableAttribArrays();
  _vao.getBuffer(0).render();
  _vao.disableAttribArrays();
}

void NewCPUPointCloud::update(std::vector<glm::vec3>& positions)
{
  _vao.getBuffer(0).update(
    {&positions[0], sizeof(glm::vec3) * positions.size(),
    static_cast<GLuint>(positions.size()), GL_FLOAT, GL_ARRAY_BUFFER,
    GL_POINTS, GL_DYNAMIC_DRAW});
}

} }
