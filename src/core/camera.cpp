#include "SGE/core/camera.h"

namespace sge { namespace core {

AbstractCamera::AbstractCamera()
{

}

AbstractCamera::~AbstractCamera()
{
  
}

void AbstractCamera::addToShader(GLuint program_ID)
{
  glUseProgram(program_ID);
  CameraShaderHandle handle;
  // Update values
  handle.view_matrix_ID =  glGetUniformLocation(program_ID, "V");
  handle.projection_matrix_ID = glGetUniformLocation(program_ID, "P");

  // Add to the map
  shader_handles_.insert(
    std::pair<GLuint, CameraShaderHandle>(program_ID, handle));
}

void AbstractCamera::removeFromShader(GLuint program_ID)
{
  shader_handles_.erase(program_ID);
}

void AbstractCamera::updateAllShaderUniforms()
{
  glm::mat4 V = glm::inverse(absoluteTransform());
  // For all shaders, push data as uniforms
  for(auto it = shader_handles_.begin(); it != shader_handles_.end(); ++it)
  {
    glUseProgram(it->first);
    glUniformMatrix4fv(
      it->second.view_matrix_ID,
      1,
      GL_FALSE,
      &V[0][0]);
    glUniformMatrix4fv(
      it->second.projection_matrix_ID,
      1,
      GL_FALSE,
      &_projection_transform[0][0]);
  }
}

glm::mat4 AbstractCamera::projectionTransform() const
{
  return _projection_transform;
}

glm::mat4 AbstractCamera::viewTransform() const
{
  return glm::inverse(absoluteTransform());
}

std::pair<glm::vec3, glm::vec3> AbstractCamera::unproject(
  const glm::vec2& position_ndc) const
{
  // Transform from [-1, 1] to [0, 1]
  glm::vec2 position = position_ndc / 2.0f + glm::vec2(0.5);
  float w = 1, h = 1;

  const glm::mat4& V = glm::inverse(absoluteTransform());
  const glm::mat4& P = _projection_transform;
  
  glm::vec3 from =
    glm::unProject(glm::vec3(position, 0.0f), V, P, glm::vec4(0, 0, w, h));
  glm::vec3 to =
    glm::unProject(glm::vec3(position, 1.0f), V, P, glm::vec4(0, 0, w, h));
  
  return {from, to - from};
}

PerspectiveCamera::PerspectiveCamera(
  float fov,
  float aspect,
  float near,
  float far) :
  _fov(fov),
  _aspect(aspect),
  _near(near),
  _far(far)
{
  updateProjectionTransform();
}

void PerspectiveCamera::execute()
{
  updateProjectionTransform();
  AbstractCamera::updateAllShaderUniforms();
}

void PerspectiveCamera::setFOV(float fov)
{
  _fov = fov;
  updateProjectionTransform();
}

void PerspectiveCamera::setNearClippingPlane(float near)
{
  _near = near;
  updateProjectionTransform();
}

void PerspectiveCamera::setFarClippingPlane(float far)
{
  _far = far;
  updateProjectionTransform();
}

void PerspectiveCamera::setAspectRatio(float aspect)
{
  _aspect = aspect;
  updateProjectionTransform();
}

void PerspectiveCamera::updateProjectionTransform()
{
  _projection_transform = glm::perspective(_fov, _aspect, _near, _far); 
}

OrthoCamera::OrthoCamera(
  float   left,
  float   right,
  float   bottom,
  float   top,
  float   near,
  float   far) :
  _left(left),
  _right(right),
  _bottom(bottom),
  _top(top),
  _near(near),
  _far(far)
{
  _projection_transform = glm::ortho(_left, _right, _bottom, _top, _near, _far);
}

void OrthoCamera::execute()
{
  _projection_transform = glm::ortho(_left, _right, _bottom, _top, _near, _far);
  AbstractCamera::updateAllShaderUniforms();
}

} }
