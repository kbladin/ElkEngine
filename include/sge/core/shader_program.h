#pragma once

#include <iostream>
#include <fstream>

#include <gl/glew.h>

namespace sge { namespace core {

class ShaderProgram {
public:
  ShaderProgram(
    std::string name,
    const char* vs_src,
    const char* tcs_src,
    const char* tes_src,
    const char* gs_src,
    const char* fs_src);
  ~ShaderProgram();

  inline void bind() { glUseProgram(_id); };
  inline void unbind() { glUseProgram(0); };

  inline const GLuint& id() { return _id; };
private:
  GLuint loadShaderProgram(
    const char* vs_src,
    const char* tcs_src,
    const char* tes_src,
    const char* gs_src,
    const char* fs_src);

  std::string _name;
  GLuint _id;
};

} }