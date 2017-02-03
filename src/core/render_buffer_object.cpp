#include "sge/core/render_buffer_object.h"

#include <gl/glew.h>

#include <iostream>

namespace sge { namespace core {

RenderBufferObject::RenderBufferObject(
	GLsizei width, GLsizei height, GLenum internalformat) :
  _width(width),
  _height(height)
{
  glGenRenderbuffers(1, &_id);
  bind();
  glRenderbufferStorage(GL_RENDERBUFFER, internalformat, _width, _height);
} 

RenderBufferObject::~RenderBufferObject()
{
  glDeleteRenderbuffers(1, &_id);
}

} }
