#pragma once

#include <vector>

#include <gl/glew.h>

namespace elk { namespace core {

class ArrayBuffer
{
public:

  // ArrayBuffer does not have ownership of the data!
  struct InitData
  {
    void* data;
    GLsizei data_size;
    GLuint n_elements;
    GLenum type;
    GLenum buffer_type;
    GLenum mode = GL_STATIC_DRAW;
    GLenum render_mode = GL_TRIANGLES;
  };

  ArrayBuffer(InitData init_data);
  ~ArrayBuffer();
  
  inline GLuint id() { return _id; };

  inline void bind() { glBindBuffer(_init_data.buffer_type, _id); };
  inline void unbind() { glBindBuffer(_init_data.buffer_type, 0); };

  void render();
  void update(InitData init_data);

protected:
  void upload();
  InitData _init_data;
  GLuint _id;
};

class ElementArrayBuffer : public ArrayBuffer
{
public:
  ElementArrayBuffer(InitData init_data);
  void render();
private:
};

} }
