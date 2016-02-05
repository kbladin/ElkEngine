#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <vector>
#include <string>

#include <GL/glew.h>

class ShaderLoader {
public:
  static GLuint loadShaders(
    const char * vertex_file_path,
    const char * tesselation_control_file_path,
    const char * tesselation_eval_file_path,
    const char * geometry_file_path,
    const char * fragment_file_path);
};

#endif