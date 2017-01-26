#pragma once

#include <iostream>
#include <fstream>
#include <stack>

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

  void pushUsage();
  void popUsage();
  void useNone();

  inline const GLuint& id() { return _id; };
  static inline const GLuint& currentProgramId() { return _shader_stack.top(); };
private:
  GLuint loadShaderProgram(
    const char* vs_src,
    const char* tcs_src,
    const char* tes_src,
    const char* gs_src,
    const char* fs_src);

  std::string _name;
  GLuint _id;
  static std::stack<GLuint> _shader_stack;
};

} }