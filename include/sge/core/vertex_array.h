#pragma once

#include "sge/core/array_buffer.h"

#include <gl/glew.h>

#include <map>

namespace sge { namespace core {

class VertexArray
{
public:
  VertexArray();
  ~VertexArray();

  void addBuffer(ArrayBuffer::InitData buffer_init_data, GLuint attribute_index,
    GLint n_components, GLenum type = GL_FLOAT, GLboolean normalized = GL_FALSE);

  inline void bind() { glBindVertexArray(_id); };
  inline void unbind() { glBindVertexArray(0); };
  ArrayBuffer& getBuffer(int attribute_index) { return *_buffers[attribute_index]; };
  void enableAttribArrays();
  void disableAttribArrays();
private:
  GLuint _id;
  std::map<int, std::unique_ptr<ArrayBuffer> > _buffers;
};

} }
