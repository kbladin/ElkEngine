#include "SGE/core/simple_graphics_engine.h"

namespace sge { namespace core {

SimpleGraphicsEngine::SimpleGraphicsEngine(int width, int height) :
  perspective_camera(45.0f, static_cast<float>(width) / height, 0.01, 100),
  viewspace_ortho_camera(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f),
  _window_width(width),
  _window_height(height)
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

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
  // Cull triangles which normal is not towards the camera
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  // Enable blending
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return true;
}

void SimpleGraphicsEngine::updateTransforms()
{
  perspective_camera.updateTransform(glm::mat4());
  viewspace_ortho_camera.updateTransform(glm::mat4());

  scene.updateTransform(glm::mat4());
  view_space.updateTransform(glm::mat4());
  background_space.updateTransform(glm::mat4());
}


void SimpleGraphicsEngine::render()
{
  updateTransforms();

  // Then render
  glClearColor(0.0, 0.0, 0.0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDisable(GL_DEPTH_TEST);
  viewspace_ortho_camera.execute();
  background_space.execute();
  glEnable(GL_DEPTH_TEST);
  perspective_camera.execute();
  scene.execute();
  glDisable(GL_DEPTH_TEST);
  viewspace_ortho_camera.execute();
  view_space.execute();

  // Check for errors
  checkForErrors();
}

void SimpleGraphicsEngine::checkForErrors()
{
  GLenum error_code = glGetError();
  if (error_code != GL_NO_ERROR)
  {
    fprintf(stderr, "OpenGL ERROR : %s\n", gluErrorString(error_code));
  }
}

int SimpleGraphicsEngine::windowWidth()
{
  return _window_width;
}

int SimpleGraphicsEngine::windowHeight()
{
  return _window_height;
}

PerspectiveCamera& SimpleGraphicsEngine::camera()
{
  return perspective_camera;
}

OrthoCamera& SimpleGraphicsEngine::viewSpaceCamera()
{
  return viewspace_ortho_camera;
}

void SimpleGraphicsEngine::setWindowResolution(int width, int height)
{
  _window_width = width;
  _window_height = height;
  glViewport(0,0,_window_width, _window_height);
  perspective_camera.setAspectRatio( static_cast<float>(width) / height);
}

} }
