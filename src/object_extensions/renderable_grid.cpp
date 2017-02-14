#include "elk/object_extensions/renderable_grid.h"
#include "elk/core/create_mesh.h"
#include "elk/core/shader_manager.h"
#include "elk/core/camera.h"

namespace elk { namespace core {

RenderableGrid::RenderableGrid()
{
  _program = std::make_shared<ShaderProgram>(
    "grid_program",
    (std::string(ELK_DIR) + "/shaders/simple_white.vert").c_str(),
    nullptr,
    nullptr,
    nullptr,
    (std::string(ELK_DIR) + "/shaders/simple_white.frag").c_str());
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
