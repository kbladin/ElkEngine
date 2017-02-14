#include "elk/core/vertex_array.h"

namespace elk { namespace core {

VertexArray::VertexArray()
{
  glGenVertexArrays(1, &_id);
}

VertexArray::~VertexArray()
{
  glDeleteVertexArrays(1, &_id);
}

void VertexArray::addBuffer(
  ArrayBuffer::InitData buffer_init_data, GLuint attribute_index,
  GLint n_components, GLenum type, GLboolean normalized)

{
  _buffers[attribute_index] = std::make_unique<ArrayBuffer>(buffer_init_data);

  bind();

  // Bind the newly added buffer
  _buffers[attribute_index]->bind();
  glVertexAttribPointer(
    attribute_index,
    n_components,
    type,
    normalized,
    0, // stride
    static_cast<void*>(0) ); // array buffer offset
}

void VertexArray::enableAttribArrays()
{
  for (auto& pair : _buffers)
  {
    glEnableVertexAttribArray(pair.first);
  }
}

void VertexArray::disableAttribArrays()
{
  for (auto& pair : _buffers)
  {
    glDisableVertexAttribArray(pair.first);
  }
}

} }