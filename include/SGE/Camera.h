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
  AbstractCamera(GLuint program_ID, int width, int height);
  virtual void render(glm::mat4 M) = 0;
  void setShader(GLuint program_ID);
  void setNearClippingPlane(float near);
  void setFarClippingPlane(float far);
  void setResolution(int width, int height);

  glm::mat4 getProjectionTransform();
protected:
  GLuint program_ID_;
  GLuint view_matrix_ID_;
  GLuint projection_matrix_ID_;
  
  glm::mat4 projection_transform_;
  
  int width_;
  int height_;
  float near_;
  float far_;
};

//! A perspective camera defined in 3D space
class PerspectiveCamera : public AbstractCamera {
public:
  PerspectiveCamera(
    GLuint program_ID,
    int width,
    int height,
    float fov,
    float near,
    float far);
  virtual void render(glm::mat4 M);
  void setFOV(float fov);
private:
  float fov_;
};

//! An orthographic camera defined in 3D space
class OrthoCamera : public AbstractCamera {
public:
  OrthoCamera(
    GLuint program_ID,
    int width,
    int height,
    float near,
    float far);
  virtual void render(glm::mat4 M);
};


#endif