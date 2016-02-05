#include "SGE/Camera.h"

//! Creates camera to render objects with the defined shader program attached.
/*!
  The shader program can later be changed so that other shaders can be
  used for this particular camera. It is also possible to have several
  cameras and switching between them to use separate shaders.
  \param program_ID is the shader program that this Camera will render.
  \param width is the width in pixels of the camera.
  \param height is the height in pixels of the camera.
*/
AbstractCamera::AbstractCamera(GLuint program_ID, int width, int height)
{
  width_ = width;
  height_ = height;
  program_ID_ = program_ID;
  glUseProgram(program_ID_);

  view_matrix_ID_ = glGetUniformLocation(program_ID_, "V");
  projection_matrix_ID_ = glGetUniformLocation(program_ID_, "P");
}

//! Render the Camera.
/*!
  Sets the view and projection matrices of the cameras shader to the
  corresponding transforms for the camera.
  \param M is the transformation matrix of the parent.
*/
void AbstractCamera::render(glm::mat4 M)
{
  glm::mat4 V = M * transform_matrix_;
  Object3D::render(V);
  
  glUseProgram(program_ID_);

  glUniformMatrix4fv(view_matrix_ID_, 1, GL_FALSE, &V[0][0]);
  glUniformMatrix4fv(projection_matrix_ID_, 1, GL_FALSE, &projection_transform_[0][0]);
}

//! Set the camera to be attached to the shader provided
/*!
  The camera will set the needed view and projection matrices when rendering.
  \param program_ID is the shader program containing the V and P matrices
  to be set by the camera
*/
void AbstractCamera::setShader(GLuint program_ID)
{
  program_ID_ = program_ID;
  glUseProgram(program_ID_);

  view_matrix_ID_ = glGetUniformLocation(program_ID_, "V");
  projection_matrix_ID_ = glGetUniformLocation(program_ID_, "P");
}

//! Set the near clipping plane of the camera
/*!
  \param near is a positive value.
*/
void AbstractCamera::setNearClippingPlane(float near)
{
  near_ = near;
}

//! Set the far clipping plane of the camera
/*!
  \param far is a positive value.
*/
void AbstractCamera::setFarClippingPlane(float far)
{
  far_ = far;
}

void AbstractCamera::setResolution(int width, int height)
{
  width_ = width;
  height_ = height;
}

//! Returns the cameras projection matrix
glm::mat4 AbstractCamera::getProjectionTransform()
{
  return projection_transform_;
}

//! Creates camera to render objects with the defined shader program attached.
/*!
  The shader program can later be changed so that other shaders can be
  used for this particular camera. It is also possible to have several
  cameras and switching between them to use separate shaders.
  \param program_ID is the shader program that this Camera will render.
  \param width is the width in pixels of the camera.
  \param height is the height in pixels of the camera.
  \param fov is the field of view in angles of the camera
  \param near is the near plane of the camera (a positive value)
  \param far is the far plane of the camera (a positive value)
*/
PerspectiveCamera::PerspectiveCamera(
    GLuint program_ID,
    int width,
    int height,
    float fov,
    float near,
    float far) :
AbstractCamera(program_ID, width, height)
{
  fov_ = fov;
  near_ = near;
  far_ = far;

  float aspect = float(width_)/height_;
  projection_transform_ = glm::perspective(fov_, aspect, near_, far_);
}

//! Render the Camera.
/*!
  Sets the view and projection matrices of the cameras shader to the
  corresponding transforms for the camera.
  \param M is the transformation matrix of the parent.
*/
void PerspectiveCamera::render(glm::mat4 M)
{
  float aspect = float(width_)/height_;
  projection_transform_ = glm::perspective(fov_, aspect, near_, far_);
  
  AbstractCamera::render(M);
}

//! Set the field of view of the camera in angles
/*!
  \param fov is a positive value.
*/
void PerspectiveCamera::setFOV(float fov)
{
  fov_ = fov;
}

//! Creates camera to render objects with the defined shader program attached.
/*!
  The shader program can later be changed so that other shaders can be
  used for this particular camera. It is also possible to have several
  cameras and switching between them to use separate shaders.
  The aspect ratio is set from the parameter \param window.
  \param program_ID is the shader program that this Camera will render.
  \param width is the width in pixels of the camera.
  \param height is the height in pixels of the camera.
  \param near is the near plane of the camera (a positive value)
  \param far is the far plane of the camera (a positive value)
*/
OrthoCamera::OrthoCamera(
  GLuint program_ID,
  int width,
  int height,
  float near,
  float far) :
  AbstractCamera(program_ID, width, height)
{
  near_ = near;
  far_ = far;

  float aspect = float(width_)/height_;
  projection_transform_ = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -far_, near_);
}

//! Render the Camera.
/*!
  Sets the view and projection matrices of the cameras shader to the
  corresponding transforms for the camera.
  \param M is the transformation matrix of the parent.
*/
void OrthoCamera::render(glm::mat4 M)
{
  float aspect = float(width_)/height_;
  projection_transform_ = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -far_, near_);
  
  AbstractCamera::render(M);
}
