#include "sge/object_extensions/renderable_grid.h"
#include "sge/core/create_mesh.h"
#include "sge/core/shader_manager.h"
#include "sge/core/camera.h"

namespace sge { namespace core {

RenderableGrid::RenderableGrid()
{
  _program = std::make_shared<ShaderProgram>(
    "grid_program",
    "../../shaders/simple_white.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/simple_white.frag");
  _mesh = CreateMesh::grid(20);
}

void RenderableGrid::render(const UsefulRenderData& render_data)
{
  _program->pushUsage();
  glUniformMatrix4fv(
      glGetUniformLocation(ShaderProgram::currentProgramId(), "M"),
      1,
      GL_FALSE,
      &absoluteTransform()[0][0]);
  glUniformMatrix4fv(
      glGetUniformLocation(ShaderProgram::currentProgramId(), "V"),
      1,
      GL_FALSE,
      &render_data.camera.viewTransform()[0][0]);
  glUniformMatrix4fv(
      glGetUniformLocation(ShaderProgram::currentProgramId(), "P"),
      1,
      GL_FALSE,
      &render_data.camera.projectionTransform()[0][0]);

  _mesh->render();
  _program->popUsage();
}

} }
