#pragma once

#include <gl/glew.h>

namespace sge { namespace core {

class RenderBufferObject
{
public:
  RenderBufferObject(GLsizei width, GLsizei height);
  ~RenderBufferObject();

  inline void bind() { glBindRenderbuffer(GL_RENDERBUFFER, _id); };
  inline void unbind() { glBindRenderbuffer(GL_RENDERBUFFER, 0); };
  inline GLuint id() { return _id; };
private:
  GLuint _id;
  GLsizei _width, _height;
};

} } 
