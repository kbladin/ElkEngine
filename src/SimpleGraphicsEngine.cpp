#include "SGE/SimpleGraphicsEngine.h"

//! Creates a LightSource bound to a specific shader.
/*!
  \param program_ID is the shader program that this LightSource will be bound
  to.
*/
LightSource::LightSource(
    GLuint program_ID,
    float intensity,
    glm::vec3 color)
{
  _program_ID =   program_ID;
  _intensity =    intensity;
  _color =        color;
  
  glUseProgram(_program_ID);

  _light_position_ID =  glGetUniformLocation(_program_ID, "light_position");
  _light_direction_ID = glGetUniformLocation(_program_ID, "light_direction");
  _light_intensityID = glGetUniformLocation(_program_ID, "light_intensity");
  _light_colorID =     glGetUniformLocation(_program_ID, "light_color");
}

LightSource::~LightSource()
{

}

//! Sets the intensity of the light source.
/*!
  \param intensity is a positive value.
*/
void LightSource::setIntensity(float intensity)
{
  _intensity = intensity;
}

//! Sets the color of the light source.
/*!
  \param color is a rgb vector with positive values.
*/
void LightSource::setColor(glm::vec3 color)
{
  _color = color;
}

//! Render the LightSource.
/*!
  Setting all the uniforms of the lightsource for rendering.
  \param M is the transformation matrix of the parent.
*/
void LightSource::render(glm::mat4 M)
{
  Object3D::render(M * transform_matrix);

  // Data to be uploaded to shader
  glm::vec4 position = M * transform_matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  
  glUseProgram(_program_ID);
  
  // Upload data to shader
  glUniform3f(_light_position_ID, position.x, position.y, position.z);
  glUniform1f(_light_intensityID, _intensity);
  glUniform3f(_light_colorID, _color.r, _color.g, _color.b);
}

//! Static camera object
/*!
  Objects of type AbstractCameras subclasses should be added as _children to
  this object 
*/
Object3D* SimpleGraphicsEngine::camera;

//! Static camera object
/*!
  Objects of type AbstractCameras subclasses which are orthogonal should be
  added as _children to this object. This camera renders objects in screen space.
*/
Object3D* SimpleGraphicsEngine::viewspace_ortho_camera;

//! Constructor
/*!
  Create an object of SimpleGraphicsEngine.
  \param time is the currens global time.
*/
SimpleGraphicsEngine::SimpleGraphicsEngine()
{
  if (!_initialize())
  {
    fprintf(stderr, "Could not initialize SimpleGraphicsEngine\n");
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

  camera =                 new Object3D();
  viewspace_ortho_camera = new Object3D();

  scene->addChild(camera); 
  view_space->addChild(viewspace_ortho_camera);
  
  return true;
}

//! 
void SimpleGraphicsEngine::render()
{
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float aspect = float(_window_width) / _window_height;
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
