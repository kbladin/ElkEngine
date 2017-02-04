#include "sge/object_extensions/renderable_model.h"
#include "sge/core/create_mesh.h"
#include "sge/core/create_texture.h"
#include "sge/core/texture_unit.h"
#include "sge/core/shader_program.h"
#include "sge/core/debug_input.h"

namespace sge { namespace core {

RenderableModel::RenderableModel(
      std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
  _mesh(mesh),
  _material(material)
{ }

void RenderableModel::render()
{
  _material->use(ShaderProgram::currentProgramId());

  glUniform1f(glGetUniformLocation(ShaderProgram::currentProgramId(),
    "roughness"), DebugInput::value("roughness"));
  glUniform1f(glGetUniformLocation(ShaderProgram::currentProgramId(),
    "IOR"), DebugInput::value("IOR"));

  glUniformMatrix4fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "M"),
    1,
    GL_FALSE,
    &absoluteTransform()[0][0]);

  _mesh->render();
}

void RenderableModel::update(double dt)
{
  Object3D::update(dt);
  setTransform(
    relativeTransform() * glm::rotate(float(dt), glm::vec3(0.0f, 1.0f, 0.0f)) );
}

} }
