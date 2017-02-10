#pragma once

#include "SGE/core/object_3d.h"

#include <map>

#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace sge { namespace core {

struct CameraShaderHandle
{
  GLuint view_matrix_ID;
  GLuint projection_matrix_ID;
};

//! A camera defined in 3D space
class AbstractCamera : public Object3D {
public:
  AbstractCamera();
  ~AbstractCamera();

  virtual void execute() = 0;
  void addToShader(GLuint program_ID);
  void removeFromShader(GLuint program_ID);

  // Getters
  glm::mat4 projectionTransform() const;
  glm::mat4 viewTransform() const;
  // Origin and direction
  std::pair<glm::vec3, glm::vec3> unproject(const glm::vec2& position_ndc) const;
protected:
  //!
  /*!
    Sets the view and projection matrices of the cameras shader to the
    corresponding transforms for the camera. Updates the uniforms "V" and "P"
    for all shaders
  */
  void updateAllShaderUniforms();
  // Data
  std::map<GLuint, CameraShaderHandle> shader_handles_;
  glm::mat4 _projection_transform;

  // Cached view matrix
  glm::mat4 _view_transform;
};

//! A perspective camera defined in 3D space
class PerspectiveCamera : public AbstractCamera {
public:
  PerspectiveCamera(
    float aspect,
    float near,
    float far,
    float diagonal = 35,
    float focal_length = 28,
    float focal_ratio = 5);
  
  virtual void execute();

  // Setters
  //! Set the field of view of the camera in angles
  /*!
    \param fov is a positive value.
  */
  //! Set the near clipping plane of the camera
  /*!
    \param near is a positive value.
  */
  void setNearClippingPlane(float near);
  //! Set the far clipping plane of the camera
  /*!
    \param far is a positive value.
  */
  void setFarClippingPlane(float far);
  void setAspectRatio(float aspect);
  void setFocalLength(float focal_length);
  void setFocalRatio(float focal_ratio);
  void setFocus(float focus);
  
  float apertureDiameter();
  // f-number
  float focalRatio();
  float focalLength();
  float focus();
  float diagonal();
private:
  void updateProjectionTransform();
  void updateFOV();
  
  // Data
  float _fov;
  float _aspect;
  float _near;    // Near plane in camera coordinates (positive value)
  float _far;     // Far plane in camera coordinates (positive value)

  float _diagonal;
  float _focal_length;
  float _focal_ratio;
  float _focus;
};

//! An orthographic camera defined in 3D space
class OrthoCamera : public AbstractCamera {
public:
  OrthoCamera(
    float left,
    float right,
    float bottom,
    float top,
    float near,
    float far);

  virtual void execute();
private:
  float _left;
  float _right;
  float _bottom;
  float _top;
  float _near;
  float _far;
};

} }
