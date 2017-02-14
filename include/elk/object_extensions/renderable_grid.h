#pragma once

#include "elk/core/object_3d.h"
#include "elk/core/mesh.h"
#include "elk/core/shader_program.h"

namespace elk { namespace core {

class RenderableGrid : public RenderableForward
{
public:
  RenderableGrid();
  ~RenderableGrid(){};
  virtual void render(const UsefulRenderData& render_data) override;
private:
  std::shared_ptr<ShaderProgram> _program;
  std::shared_ptr<Mesh> _mesh;
};

} }
