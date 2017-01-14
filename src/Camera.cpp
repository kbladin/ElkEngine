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

//! Render the Camera.
/*!
  Sets the view and projection matrices of the cameras shader to the
  corresponding transforms for the camera.
  \param M is the transformation matrix of the parent.
*/
void AbstractCamera::execute()
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

//! Returns the cameras projection matrix
const glm::mat4& AbstractCamera::projectionTransform()
{
  return _projection_transform;
}

void AbstractCamera::unproject(
  glm::vec2 position_ndc,  glm::vec3* origin, glm::vec3* direction) const
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
  *origin = from;
  *direction = glm::normalize(to - from);
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
  float fov,
  float aspect,
  float near,
  float far) :
  _fov(fov),
  _aspect(aspect),
  _near(near),
  _far(far)
{
  _projection_transform = glm::perspective(_fov, _aspect, _near, _far);
}

//! Render the Camera.
/*!
  Sets the view and projection matrices of the cameras shader to the
  corresponding transforms for the camera.
  \param M is the transformation matrix of the parent.
*/
void PerspectiveCamera::execute()
{
  _projection_transform = glm::perspective(_fov, _aspect, _near, _far);
  AbstractCamera::execute();
}

//! Set the field of view of the camera in angles
/*!
  \param fov is a positive value.
*/
void PerspectiveCamera::setFOV(float fov)
{
  _fov = fov;
}

//! Set the near clipping plane of the camera
/*!
  \param near is a positive value.
*/
void PerspectiveCamera::setNearClippingPlane(float near)
{
  _near = near;
}

//! Set the far clipping plane of the camera
/*!
  \param far is a positive value.
*/
void PerspectiveCamera::setFarClippingPlane(float far)
{
  _far = far;
}

void PerspectiveCamera::setAspectRatio(float aspect)
{
  _aspect = aspect;
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

//! Render the Camera.
/*!
  Sets the view and projection matrices of the cameras shader to the
  corresponding transforms for the camera.
  \param M is the transformation matrix of the parent.
*/
void OrthoCamera::execute()
{
  _projection_transform = glm::ortho(_left, _right, _bottom, _top, _near, _far);
  AbstractCamera::execute();
}
