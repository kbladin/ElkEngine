#include "SGE/SimpleGraphicsEngine.h"

//! Constructor
/*!
  Create an object of SimpleGraphicsEngine.
  \param time is the currens global time.
*/
SimpleGraphicsEngine::SimpleGraphicsEngine(int width, int height) :
  perspective_camera(45, static_cast<float>(width) / height, 0.01, 100),
  viewspace_ortho_camera(-1,1,-1,1,1,-1)
{
  setWindowResolution(width, height);
  if (!_initializeGL())
  {
    fprintf(stderr, "Could not initialize SimpleGraphicsEngine. Is an OpenGL context created?\n");
  }
}

//! Destructor
SimpleGraphicsEngine::~SimpleGraphicsEngine()
{

} 

//! Initializes OpenGL, creating context and adding all basic objects for the scene.
bool SimpleGraphicsEngine::_initializeGL()
{
  glewExperimental = true; // Needed in core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return false;
  }
  
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
  // Cull triangles which normal is not towards the camera
  glEnable(GL_CULL_FACE);
  // Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return true;
}

//! 
void SimpleGraphicsEngine::render()
{
  // First update
  perspective_camera.update(glm::mat4());
  viewspace_ortho_camera.update(glm::mat4());

  scene.update(glm::mat4());
  view_space.update(glm::mat4());
  background_space.update(glm::mat4());

  // Then render
  glClearColor(0.0, 0.0, 0.0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,_window_width, _window_height);

  glDisable(GL_DEPTH_TEST);
  viewspace_ortho_camera.execute();
  background_space.execute();
  glEnable(GL_DEPTH_TEST);
  perspective_camera.execute();
  scene.execute();
  glDisable(GL_DEPTH_TEST);
  viewspace_ortho_camera.execute();
  view_space.execute();
}

//! Returns the window width   
int SimpleGraphicsEngine::windowWidth()
{
  return _window_width;
}

//! Returns the window height
int SimpleGraphicsEngine::windowHeight()
{
  return _window_height;
}

const PerspectiveCamera& SimpleGraphicsEngine::camera()
{
  return perspective_camera;
}

const OrthoCamera& SimpleGraphicsEngine::viewSpaceCamera()
{
  return viewspace_ortho_camera;
}

//! Set resolution to new window width and height
/*!
  \param width is the new window width.
  \param height is the new window height.
*/
void SimpleGraphicsEngine::setWindowResolution(int width, int height)
{
  _window_width = width;
  _window_height = height;
  perspective_camera.setAspectRatio( static_cast<float>(width) / height);
}
