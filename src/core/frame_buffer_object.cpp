#include "sge/core/frame_buffer_object.h"

#include <gl/glew.h>

#include <iostream>

namespace sge { namespace core {

FrameBufferObject::FrameBufferObject()
{
  glGenFramebuffers(1, &_id);
}

FrameBufferObject::~FrameBufferObject()
{
  glDeleteFramebuffers(1, &_id);
}

void FrameBufferObject::bind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, _id);
}

void FrameBufferObject::unbind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObject::attach2DTexture(
  GLuint texture_id, GLint attachment, GLint level)
{
  bind();
  glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture_id, level);
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "ERROR: Framebuffer not complete! Was texture uploaded?" << std::endl;
  }
  unbind();
}

void FrameBufferObject::attachRenderBuffer(GLuint render_buffer_id, GLint attachment)
{
  bind();
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, render_buffer_id);
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "ERROR: Framebuffer not complete!" << std::endl;
  }
  unbind();
}

} }
