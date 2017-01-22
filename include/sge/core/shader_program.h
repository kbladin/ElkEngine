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
private:
  std::string _name;
};

} }