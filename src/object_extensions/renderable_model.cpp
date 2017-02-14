#include "elk/object_extensions/renderable_model.h"
#include "elk/core/create_mesh.h"
#include "elk/core/create_texture.h"
#include "elk/core/texture_unit.h"
#include "elk/core/shader_program.h"
#include "elk/core/debug_input.h"
#include "elk/core/camera.h"

namespace elk { namespace core {

RenderableModel::RenderableModel(
      std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
  _mesh(mesh),
  _material(material)
{ }

void RenderableModel::render(const UsefulRenderData& render_data)
{
  _material->use();

  glUniformMatrix4fv(
    glGetUniformLocation(_material->programId(), "M"),
    1,
    GL_FALSE,
    &absoluteTransform()[0][0]);
  glUniformMatrix4fv(
      glGetUniformLocation(_material->programId(), "V"),
      1,
      GL_FALSE,
      &render_data.camera.viewTransform()[0][0]);
  glUniformMatrix4fv(
      glGetUniformLocation(_material->programId(), "P"),
      1,
      GL_FALSE,
      &render_data.camera.projectionTransform()[0][0]);

  _mesh->render();
}

void RenderableModel::update(double dt)
{
  Object3D::update(dt);
  //setTransform(
  //  relativeTransform() * glm::rotate(float(dt) * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f)) );
}

} }
