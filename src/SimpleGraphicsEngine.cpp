#include "SGE/SimpleGraphicsEngine.h"

//! Constructor
/*!
  Create an object of SimpleGraphicsEngine.
  \param time is the currens global time.
*/
SimpleGraphicsEngine::SimpleGraphicsEngine(int width, int height)
{
  setWindowResolution(width, height);
  if (!_initialize())
  {
    fprintf(stderr, "Could not initialize SimpleGraphicsEngine. Is an OpenGL context created?\n");
  }
}

//! Destructor
SimpleGraphicsEngine::~SimpleGraphicsEngine()
{
  delete scene;
  delete view_space;
  delete background_space;  
  
  delete camera;
  delete viewspace_ortho_camera;
} 

//! Initializes OpenGL, creating context and adding all basic objects for the scene.
bool SimpleGraphicsEngine::_initialize()
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
  
  scene =             new Object3D();
  view_space =        new Object3D();
  background_space =  new Object3D();

  camera =                 new PerspectiveCamera(45, 2.0, 0.01, 100);
  viewspace_ortho_camera = new OrthoCamera(-1,1,-1,1,1,-1);

  scene->addChild(camera);
  view_space->addChild(viewspace_ortho_camera);
  
  return true;
}

//! 
void SimpleGraphicsEngine::render()
{
  glClearColor(0.0, 0.0, 0.0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0,0,_window_width * 2, _window_height * 2);

  glDisable(GL_DEPTH_TEST);
  background_space->render(glm::mat4());
  glEnable(GL_DEPTH_TEST);
  scene->render(glm::mat4());
  view_space->render(glm::mat4());
}

//! Returns the window width   
int SimpleGraphicsEngine::getWindowWidth()
{
  return _window_width;
}

//! Returns the window height
int SimpleGraphicsEngine::getWindowHeight()
{
  return _window_height;
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
}
