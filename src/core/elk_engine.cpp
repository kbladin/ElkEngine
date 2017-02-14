#include "elk/core/elk_engine.h"

namespace elk { namespace core {

ElkEngine::ElkEngine() :
  perspective_camera(1.0, 0.01, 100),
  viewspace_ortho_camera(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f)
{
  if (!_initializeGL())
  {
    fprintf(stderr, "Could not initialize ElkEngine. Is an OpenGL context created?\n");
  }
}

ElkEngine::~ElkEngine()
{

} 

bool ElkEngine::_initializeGL()
{
  glewExperimental = true; // Needed in core profile

  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return false;
  }
  // Call glGetError to flush first error code that appears due to an error in glew
  glGetError();

  return true;
}

void ElkEngine::update(double dt)
{
  // Call update for all objects
  scene.update(dt);
  view_space.update(dt);
  background_space.update(dt);

  // Update all transforms
  perspective_camera.updateTransform(glm::mat4());
  viewspace_ortho_camera.updateTransform(glm::mat4());

  scene.updateTransform(glm::mat4());
  view_space.updateTransform(glm::mat4());
  background_space.updateTransform(glm::mat4());
}

PerspectiveCamera& ElkEngine::camera()
{
  return perspective_camera;
}

OrthoCamera& ElkEngine::viewSpaceCamera()
{
  return viewspace_ortho_camera;
}

} }
