#include "sge/core/render_buffer_object.h"

#include <gl/glew.h>

#include <iostream>

namespace sge { namespace core {

RenderBufferObject::RenderBufferObject(GLsizei width, GLsizei height) :
  _width(width),
  _height(height)
{
  glGenRenderbuffers(1, &_id);
  bind();
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);
} 

RenderBufferObject::~RenderBufferObject()
{
  glDeleteRenderbuffers(1, &_id);
}

} }
