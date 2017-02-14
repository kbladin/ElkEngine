#pragma once

#include "elk/core/shader_program.h"

#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include <gl/glew.h>

namespace elk { namespace core {
//! A class containing all shader program IDs in a map.
/*!
  This class uses the singleton design pattern. This means that It is not
  possible to create instances of the class. There is only one instance that
  is reached from the static function instance().
*/

  /*
class ShaderManager {
public:
  static ShaderManager& instance();
  ~ShaderManager();
  void loadAndAddShader(
    std::string name,
    const char* vs_src,
    const char* tcs_src,
    const char* tes_src,
    const char* gs_src,
    const char* fs_src);
  std::shared_ptr<ShaderProgram> getShader(std::string name);
private:
  ShaderManager();

  static ShaderManager _instance;
  std::map<std::string, std::shared_ptr<ShaderProgram> > _shader_programs;
};
*/
} }
