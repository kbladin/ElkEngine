#ifndef CAMERA_H
#define CAMERA_H

#include "SGE/Object3D.h"

#include <map>

#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

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
  const glm::mat4& projectionTransform();
  void unproject(
    glm::vec2 position_ndc, glm::vec3* origin, glm::vec3* direction) const;
protected:
  // OpenGL handles
  std::map<GLuint, CameraShaderHandle> shader_handles_;

  // Data
  glm::mat4 _projection_transform;
};

//! A perspective camera defined in 3D space
class PerspectiveCamera : public AbstractCamera {
public:
  PerspectiveCamera(
    float fov,
    float aspect,
    float near,
    float far);
  
  virtual void execute();

  // Setters
  void setFOV(float fov);
  void setNearClippingPlane(float near);
  void setFarClippingPlane(float far);
  void setAspectRatio(float aspect);
private:
  // Data
  float _fov;
  float _aspect;
  float _near;    // Near plane in camera coordinates (positive value)
  float _far;     // Far plane in camera coordinates (positive value)
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

#endif