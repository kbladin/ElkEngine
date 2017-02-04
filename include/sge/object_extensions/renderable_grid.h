#pragma once

#include "sge/core/object_3d.h"
#include "sge/core/mesh.h"
#include "sge/core/shader_program.h"

namespace sge { namespace core {

class RenderableGrid : public Renderable
{
public:
  RenderableGrid();
  ~RenderableGrid(){};
  virtual void render() override;
private:
  std::shared_ptr<ShaderProgram>  _program;
  std::shared_ptr<Mesh>        _mesh;
};

} }
