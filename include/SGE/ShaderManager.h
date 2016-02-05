#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "SGE/ShaderLoader.h"

#include <map>
#include <iostream>

#include <gl/glew.h>

//! A class containing all shader program IDs in a map.
/*!
  This class uses the singleton design pattern. This means that It is not
  possible to create instances of the class. There is only one instance that
  is reached from the static function instance().
*/
class ShaderManager {
public:
  static ShaderManager* instance();
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
  static ShaderManager* instance_;
  std::map<std::string, GLuint> shader_program_IDs;
};


#endif