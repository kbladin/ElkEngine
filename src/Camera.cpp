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
  _width = width;
  _height = height;
  _program_ID = program_ID;
  glUseProgram(_program_ID);

  _view_matrix_ID = glGetUniformLocation(_program_ID, "V");
  _projection_matrix_ID = glGetUniformLocation(_program_ID, "P");
}

AbstractCamera::~AbstractCamera()
{
  
}

//! Render the Camera.
/*!
  Sets the view and projection matrices of the cameras shader to the
  corresponding transforms for the camera.
  \param M is the transformation matrix of the parent.
*/
void AbstractCamera::render(glm::mat4 M)
{
  glm::mat4 V = M * transform_matrix;
  Object3D::render(V);
  
  glUseProgram(_program_ID);

  glUniformMatrix4fv(_view_matrix_ID, 1, GL_FALSE, &V[0][0]);
  glUniformMatrix4fv(_projection_matrix_ID, 1, GL_FALSE, &_projection_transform[0][0]);
}

//! Set the camera to be attached to the shader provided
/*!
  The camera will set the needed view and projection matrices when rendering.
  \param program_ID is the shader program containing the V and P matrices
  to be set by the camera
*/
void AbstractCamera::setShader(GLuint program_ID)
{
  _program_ID = program_ID;
  glUseProgram(_program_ID);

  _view_matrix_ID = glGetUniformLocation(_program_ID, "V");
  _projection_matrix_ID = glGetUniformLocation(_program_ID, "P");
}

//! Set the near clipping plane of the camera
/*!
  \param near is a positive value.
*/
void AbstractCamera::setNearClippingPlane(float near)
{
  _near = near;
}

//! Set the far clipping plane of the camera
/*!
  \param far is a positive value.
*/
void AbstractCamera::setFarClippingPlane(float far)
{
  _far = far;
}

void AbstractCamera::setResolution(int width, int height)
{
  _width = width;
  _height = height;
}

//! Returns the cameras projection matrix
glm::mat4 AbstractCamera::getProjectionTransform()
{
  return _projection_transform;
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
  _fov = fov;
  _near = near;
  _far = far;

  float aspect = float(_width)/_height;
  _projection_transform = glm::perspective(_fov, aspect, _near, _far);
}

//! Render the Camera.
/*!
  Sets the view and projection matrices of the cameras shader to the
  corresponding transforms for the camera.
  \param M is the transformation matrix of the parent.
*/
void PerspectiveCamera::render(glm::mat4 M)
{
  float aspect = float(_width)/_height;
  _projection_transform = glm::perspective(_fov, aspect, _near, _far);
  
  AbstractCamera::render(M);
}

//! Set the field of view of the camera in angles
/*!
  \param fov is a positive value.
*/
void PerspectiveCamera::setFOV(float fov)
{
  _fov = fov;
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
  _near = near;
  _far = far;

  float aspect = float(_width)/_height;
  _projection_transform = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -_far, _near);
}

//! Render the Camera.
/*!
  Sets the view and projection matrices of the cameras shader to the
  corresponding transforms for the camera.
  \param M is the transformation matrix of the parent.
*/
void OrthoCamera::render(glm::mat4 M)
{
  float aspect = float(_width)/_height;
  _projection_transform = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -_far, _near);
  
  AbstractCamera::render(M);
}
