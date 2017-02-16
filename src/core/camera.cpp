#include "elk/core/camera.h"

#include <glm/glm.hpp>

#include <iostream>

namespace elk { namespace core {

AbstractCamera::AbstractCamera()
{

}

AbstractCamera::~AbstractCamera()
{
  
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
  float aspect,
  float near,
  float far,
  float diagonal,
  float focal_length,
  float focal_ratio) :
  _aspect(aspect),
  _near(near),
  _far(far),
  _diagonal(diagonal),
  _focus(0.0f)
{
  setFocalLength(focal_length);
  setFocalRatio(focal_ratio);
  updateProjectionTransform();
}

void PerspectiveCamera::updateFOV()
{
  _fov = (2.0f * glm::atan(_diagonal / (2.0f * _focal_length)));
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

void PerspectiveCamera::setFocalLength(float focal_length)
{
  focal_length = glm::max(focal_length, 1.0f);
  _focal_length = focal_length;
  updateFOV();
}

void PerspectiveCamera::setFocalRatio(float focal_ratio)
{
  focal_ratio = glm::max(focal_ratio, 1.0f);
  _focal_ratio = focal_ratio;
}

void PerspectiveCamera::setFocus(float focus)
{
  focus = glm::clamp(focus, 0.0f, _focal_length);
  _focus = focus;
}

float PerspectiveCamera::apertureDiameter()
{
  return _focal_length / _focal_ratio;
}

float PerspectiveCamera::focalRatio()
{
  return _focal_ratio;
}

float PerspectiveCamera::focalLength()
{
  return _focal_length;
}
  
float PerspectiveCamera::focus()
{
  return _focus;
}

float PerspectiveCamera::diagonal()
{
  return _diagonal;
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

} }
