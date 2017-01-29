#pragma once

#include "sge/core/object_3d.h"
#include "sge/core/new_mesh.h"
#include "sge/core/shader_program.h"

namespace sge { namespace core {

class RenderableGrid : public Object3D
{
public:
  RenderableGrid();
  ~RenderableGrid(){};
  virtual void execute() override;
private:
  std::shared_ptr<ShaderProgram>  _program;
  std::shared_ptr<NewMesh>        _mesh;
};

} }
