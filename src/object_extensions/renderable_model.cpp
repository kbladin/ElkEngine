#include "sge/object_extensions/renderable_model.h"
#include "sge/core/create_mesh.h"
#include "sge/core/create_texture.h"
#include "sge/core/texture_unit.h"
#include "sge/core/shader_program.h"
#include "sge/core/debug_input.h"

namespace sge { namespace core {

RenderableModel::RenderableModel(const char* mesh_path)
{
  _new_mesh = CreateMesh::load(mesh_path);
  _material = std::make_unique<Material>(
    CreateTexture::load("../../data/textures/albedo.png"),
    CreateTexture::load("../../data/textures/roughness.png"));
}

void RenderableModel::render()
{
  //setTransform( glm::rotate(0.01f, glm::vec3(0.0f, 1.0f, 0.0f)) * relativeTransform() );

  _material->use(ShaderProgram::currentProgramId());

  glUniform1f(glGetUniformLocation(ShaderProgram::currentProgramId(), "roughness"), DebugInput::value("roughness"));
  glUniform1f(glGetUniformLocation(ShaderProgram::currentProgramId(), "IOR"), DebugInput::value("IOR"));

  glUniformMatrix4fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "M"),
    1,
    GL_FALSE,
    &absoluteTransform()[0][0]);

  _new_mesh->render();
}

} }
