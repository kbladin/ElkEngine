#include "sge/object_extensions/renderable_grid.h"
#include "sge/core/create_mesh.h"
#include "sge/core/shader_manager.h"

namespace sge { namespace core {

RenderableGrid::RenderableGrid()
{
  _mesh = CreateMesh::grid(20);
}

void RenderableGrid::render()
{
  glUniformMatrix4fv(
      glGetUniformLocation(ShaderProgram::currentProgramId(), "M"),
      1,
      GL_FALSE,
      &absoluteTransform()[0][0]);

  _mesh->render();
}

} }
