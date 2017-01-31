#include "sge/object_extensions/light_source.h"

#include "sge/core/deferred_shading_renderer.h"
#include "sge/core/create_mesh.h"

namespace sge { namespace core {

LightSource::LightSource(glm::vec3 color, float intensity) :
  Object3D(),
  _color(color)
{
  setIntensity(intensity);
  _quad_mesh = CreateMesh::quad();
  _sphere_mesh = CreateMesh::lonLatSphere(16, 8);
}

void LightSource::submit(DeferredShadingRenderer& renderer)
{
  Object3D::submit(renderer);
  renderer.submitLightSource(*this);
}

void LightSource::render(const PerspectiveCamera& camera)
{
  // If we are inside the sphere affected by the light source, render a full
  // quad. Otherwize just render the light sphere
  glm::vec4 position_world_space = glm::vec4(absoluteTransform()[3]);
  glm::vec3 position_view_space = glm::vec3(camera.viewTransform() * position_world_space);
  float distance_to_light_source = glm::length(position_view_space);
  if (distance_to_light_source < _sphere_scale * absoluteTransform()[0][0])
    renderQuad(camera);
  else
    renderSphere(camera);
}

void LightSource::renderQuad(const PerspectiveCamera& camera)
{
  // This transform is equivalent to inv(V) * inv(P) which means that after
  // transformation, the result is P * V * inv(V) * inv(P) = I. This puts the quad in view
  // space
  const glm::mat4& model_transform = camera.absoluteTransform() * glm::inverse(camera.projectionTransform());
  glUniformMatrix4fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "M"),
    1,
    GL_FALSE,
    &model_transform[0][0]);

  setupLightSourceUniforms(camera);

  _quad_mesh->render();
}

void LightSource::renderSphere(const PerspectiveCamera& camera)
{
  glm::mat4 scaled_transform = absoluteTransform();
  scaled_transform[0][0] *= _sphere_scale;
  scaled_transform[1][1] *= _sphere_scale;
  scaled_transform[2][2] *= _sphere_scale;
  glUniformMatrix4fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "M"),
    1,
    GL_FALSE,
    &scaled_transform[0][0]);

  setupLightSourceUniforms(camera);

  _sphere_mesh->render();
}

void LightSource::setupLightSourceUniforms(const PerspectiveCamera& camera)
{
  glm::vec4 position_world_space = glm::vec4(absoluteTransform()[3]);
  glm::vec4 position_view_space = camera.viewTransform() * position_world_space;

  glUniform3f(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "light_source.position"),
    position_view_space.x, position_view_space.y, position_view_space.z);
  glUniform3f(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "light_source.color"),
    _color.r, _color.g, _color.b);
  glUniform1f(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "light_source.intensity"),
    _intensity);
}

void LightSource::setIntensity(float intensity)
{
  _intensity = intensity;
  _sphere_scale = _intensity * 16 / 2; // sqrt(256) / 2
}

void LightSource::setColor(glm::vec3 color)
{
  _color = color;
}

} }
