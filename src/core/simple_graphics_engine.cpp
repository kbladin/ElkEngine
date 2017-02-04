#include "SGE/core/simple_graphics_engine.h"

namespace sge { namespace core {

SimpleGraphicsEngine::SimpleGraphicsEngine() :
  perspective_camera(45.0f, 1.0, 0.01, 100),
  viewspace_ortho_camera(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f)
{
  if (!_initializeGL())
  {
    fprintf(stderr, "Could not initialize SimpleGraphicsEngine. Is an OpenGL context created?\n");
  }
}

SimpleGraphicsEngine::~SimpleGraphicsEngine()
{

} 

bool SimpleGraphicsEngine::_initializeGL()
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

void SimpleGraphicsEngine::update(double dt)
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

PerspectiveCamera& SimpleGraphicsEngine::camera()
{
  return perspective_camera;
}

OrthoCamera& SimpleGraphicsEngine::viewSpaceCamera()
{
  return viewspace_ortho_camera;
}

} }
