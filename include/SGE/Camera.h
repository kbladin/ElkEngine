#ifndef CAMERA_H
#define CAMERA_H

#include "SGE/Object3D.h"

#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

//! A camera defined in 3D space
class AbstractCamera : public Object3D {
public:
  AbstractCamera(GLuint program_ID, float aspect);
  ~AbstractCamera();

  virtual void render(glm::mat4 M) = 0;

  // Getters
  glm::mat4 getProjectionTransform();

  // Setters
  void setShader(GLuint program_ID);
  void setNearClippingPlane(float near);
  void setFarClippingPlane(float far);
  void setAspectRatio(float aspect);

protected:
  // OpenGL handles
  GLuint _program_ID;
  GLuint _view_matrix_ID;
  GLuint _projection_matrix_ID;
  
  // Data
  glm::mat4 _projection_transform;
  float     _aspect;
  float     _near;    // Near plane in camera coordinates (positive value)
  float     _far;     // Far plane in camera coordinates (positive value)
};

//! A perspective camera defined in 3D space
class PerspectiveCamera : public AbstractCamera {
public:
  PerspectiveCamera(
    GLuint  program_ID,
    float   aspect,
    float   fov,
    float   near,
    float   far);
  
  virtual void render(glm::mat4 M);
  
  // Setters
  void setFOV(float fov);
private:
  // Data
  float _fov;
};

//! An orthographic camera defined in 3D space
class OrthoCamera : public AbstractCamera {
public:
  OrthoCamera(
    GLuint  program_ID,
    float   aspect,
    float   near,
    float   far);

  virtual void render(glm::mat4 M);
};

#endif