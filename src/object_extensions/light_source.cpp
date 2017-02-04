#include "sge/object_extensions/light_source.h"

#include "sge/core/renderer.h"
#include "sge/core/create_mesh.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace sge { namespace core {

PointLightSource::PointLightSource(glm::vec3 color, float radiant_flux) :
  Object3D(),
  _color(color)
{
  setRadiantFlux(radiant_flux);
  _quad_mesh = CreateMesh::quad();
  _sphere_mesh = CreateMesh::lonLatSphere(16, 8);
}

void PointLightSource::submit(Renderer& renderer)
{
  Object3D::submit(renderer);
  renderer.submitPointLightSource(*this);
}

void PointLightSource::render(const PerspectiveCamera& camera)
{
  // If we are inside the sphere affected by the light source, render a full
  // quad. Otherwize just render the light sphere
  glm::vec4 position_world_space = glm::vec4(absoluteTransform()[3]);
  glm::vec3 position_view_space =
    glm::vec3(camera.viewTransform() * position_world_space);
  float distance_to_light_source = glm::length(position_view_space);
  
  // Probably not the best way of handling rescaled light sources,
  // but works for now..
  glm::vec3 scale;
  glm::quat rotation;
  glm::vec3 translation;
  glm::vec3 skew;
  glm::vec4 perspective;
  glm::decompose(
    absoluteTransform(), scale, rotation, translation, skew,perspective);
  float transform_scale = scale.x;

  if (distance_to_light_source < _sphere_scale * transform_scale)
    renderQuad(camera);
  else
    renderSphere(camera);
}

void PointLightSource::renderQuad(const PerspectiveCamera& camera)
{
  // This transform is equivalent to inv(V) * inv(P) which means that after
  // transformation, the result is P * V * inv(V) * inv(P) = I.
  // This puts the quad in view space
  const glm::mat4& model_transform =
    camera.absoluteTransform() * glm::inverse(camera.projectionTransform());
  glUniformMatrix4fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "M"),
    1,
    GL_FALSE,
    &model_transform[0][0]);

  setupLightSourceUniforms(camera);

  _quad_mesh->render();
}

void PointLightSource::renderSphere(const PerspectiveCamera& camera)
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

void PointLightSource::setupLightSourceUniforms(const PerspectiveCamera& camera)
{
  glm::vec4 position_world_space = glm::vec4(absoluteTransform()[3]);
  glm::vec4 position_view_space = camera.viewTransform() * position_world_space;

  glUniform3f(
    glGetUniformLocation(ShaderProgram::currentProgramId(),
      "light_source.position"),
    position_view_space.x, position_view_space.y, position_view_space.z);
  glUniform3f(
    glGetUniformLocation(ShaderProgram::currentProgramId(),
      "light_source.color"),
    _color.r, _color.g, _color.b);
  glUniform1f(
    glGetUniformLocation(ShaderProgram::currentProgramId(),
      "light_source.radiant_flux"),
    _radiant_flux);
}

void PointLightSource::setRadiantFlux(float radiant_flux)
{
  _radiant_flux = radiant_flux;
  _sphere_scale = _radiant_flux * 16 / 2 * (M_PI * 2); // sqrt(256) / 2
}

void PointLightSource::setColor(glm::vec3 color)
{
  _color = color;
}

DirectionalLightSource::DirectionalLightSource(glm::vec3 color, float radiance) :
  Object3D(),
  _color(color),
  _radiance(radiance)
{
  _quad_mesh = CreateMesh::quad();
}

void DirectionalLightSource::submit(Renderer& renderer)
{
  Object3D::submit(renderer);
  renderer.submitDirectionalLightSource(*this);
}

void DirectionalLightSource::render(const PerspectiveCamera& camera)
{
  // This transform is equivalent to inv(V) * inv(P) which means that after
  // transformation, the result is P * V * inv(V) * inv(P) = I.
  // This puts the quad in view space
  const glm::mat4& model_transform =
    camera.absoluteTransform() * glm::inverse(camera.projectionTransform());
  glUniformMatrix4fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "M"),
    1,
    GL_FALSE,
    &model_transform[0][0]);

  setupLightSourceUniforms(camera);

  _quad_mesh->render();
}

void DirectionalLightSource::setupLightSourceUniforms(const PerspectiveCamera& camera)
{
  glm::vec3 direction_model_space = glm::vec3(0.0f, -1.0f, 0.0f);
  glm::vec3 direction_world_space =
    glm::mat3(absoluteTransform()) * direction_model_space;
  glm::vec3 direction_view_space =
    glm::mat3(camera.viewTransform()) * direction_world_space;

  glUniform3f(
    glGetUniformLocation(ShaderProgram::currentProgramId(),
      "light_source.direction"),
    direction_view_space.x, direction_view_space.y, direction_view_space.z);
  glUniform3f(
    glGetUniformLocation(ShaderProgram::currentProgramId(),
      "light_source.color"),
    _color.r, _color.g, _color.b);
  glUniform1f(
    glGetUniformLocation(ShaderProgram::currentProgramId(),
      "light_source.radiance"),
    _radiance);
}

void DirectionalLightSource::setRadiance(float radiance)
{
  _radiance = radiance;
}

void DirectionalLightSource::setColor(glm::vec3 color)
{
  _color = color;
}

} }
