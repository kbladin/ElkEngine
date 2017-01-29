#include "sge/object_extensions/renderable_grid.h"
#include "sge/core/create_mesh.h"
#include "sge/core/shader_manager.h"

namespace sge { namespace core {

RenderableGrid::RenderableGrid()
{
  _program = ShaderManager::instance().getShader("simple_white");
  _mesh = CreateMesh::grid(20);
}

void RenderableGrid::execute()
{
  _program->pushUsage();
  
    glUniformMatrix4fv(
      glGetUniformLocation(_program->id(), "M"),
      1,
      GL_FALSE,
      &absoluteTransform()[0][0]);

  _mesh->render();
  
  _program->popUsage();
}

} }
