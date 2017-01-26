#pragma once

#include "SGE/core/object_3d.h"
#include "SGE/core/shader_program.h"

namespace sge { namespace core {

class Renderer {
public:
  Renderer(ShaderProgram& program);
  ~Renderer();
  
  void render(Object3D& scene);
private:
  void checkForErrors();

  ShaderProgram& _program; 
};

} }
