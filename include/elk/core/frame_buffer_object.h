#pragma once

#include <gl/glew.h>

#include "elk/core/texture.h"

#include <map>

namespace elk { namespace core {

class FrameBufferObject
{
public:
  FrameBufferObject();
  ~FrameBufferObject();

  void bind();
  void unbind();

  void attach2DTexture(GLuint texture_id, GLint attachment, GLint level);
  void attachRenderBuffer(GLuint render_buffer_id, GLint attachment);
private:
  GLuint _id;

  static unsigned int _max_attachments;
};

} } 