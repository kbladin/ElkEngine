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

void PointLightSource::update(double dt)
{
  Object3D::update(dt);
  //setTransform(
  //  relativeTransform() * glm::rotate(float(dt) * 0.1f, glm::vec3(1.0f, 1.0f, 0.0f)) );
}

void PointLightSource::render(const UsefulRenderData& render_data)
{
  // If we are inside the sphere affected by the light source, render a full
  // quad. Otherwize just render the light sphere
  glm::vec4 position_world_space = glm::vec4(absoluteTransform()[3]);
  glm::vec3 position_view_space =
    glm::vec3(render_data.camera.viewTransform() * position_world_space);
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
    renderQuad(render_data);
  else
    renderSphere(render_data);
}

void PointLightSource::renderQuad(const UsefulRenderData& render_data)
{
  setupLightSourceUniforms(render_data);
  _quad_mesh->render();
}

void PointLightSource::renderSphere(const UsefulRenderData& render_data)
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

  setupLightSourceUniforms(render_data);

  _sphere_mesh->render();
}

void PointLightSource::setupLightSourceUniforms(const UsefulRenderData& render_data)
{
  glm::vec4 position_world_space = glm::vec4(absoluteTransform()[3]);
  glm::vec4 position_view_space = render_data.camera.viewTransform() * position_world_space;

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

void DirectionalLightSource::update(double dt)
{
  Object3D::update(dt);
  //setTransform(
  //  relativeTransform() * glm::rotate(float(dt) * 0.5f, glm::vec3(1.0f, 1.0f, 0.0f)) );
}

void DirectionalLightSource::render(const UsefulRenderData& render_data)
{
  setupLightSourceUniforms(render_data);
  _quad_mesh->render();
}

void DirectionalLightSource::setupLightSourceUniforms(const UsefulRenderData& render_data)
{
  glm::vec3 direction_model_space = glm::vec3(0.0f, -1.0f, 0.0f);
  glm::vec3 direction_world_space =
    glm::mat3(absoluteTransform()) * direction_model_space;
  glm::vec3 direction_view_space =
    glm::mat3(render_data.camera.viewTransform()) * direction_world_space;

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
