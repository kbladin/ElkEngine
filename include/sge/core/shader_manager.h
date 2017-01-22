#pragma once

#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include <gl/glew.h>

namespace sge { namespace core {
//! A class containing all shader program IDs in a map.
/*!
  This class uses the singleton design pattern. This means that It is not
  possible to create instances of the class. There is only one instance that
  is reached from the static function instance().
*/
class ShaderManager {
public:
  static ShaderManager& instance();
  ~ShaderManager();
  void loadShader(
    std::string name,
    const char* vs_src,
    const char* tcs_src,
    const char* tes_src,
    const char* gs_src,
    const char* fs_src);
  GLuint getShader(std::string name);
private:
  ShaderManager();
  GLuint _compileShader(
    const char* vertex_file_path,
    const char* tesselation_control_file_path,
    const char* tesselation_eval_file_path,
    const char* geometry_file_path,
    const char* fragment_file_path);
  
  static ShaderManager _instance;
  std::map<std::string, GLuint> _shader_program_IDs;
};

} }