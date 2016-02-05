#include "SGE/SimpleGraphicsEngine.h"

std::map<GLuint, int> LightSource::point_light_counters_;
std::map<GLuint, int> LightSource::directional_counters_;

//! Creates a LightSource bound to a specific shader.
/*!
  \param program_ID is the shader program that this LightSource will be bound
  to.
*/
LightSource::LightSource(
    GLuint program_ID,
    float intensity,
    glm::vec3 color,
    bool directional) : directional_(directional)
{
  program_ID_ =   program_ID;
  intensity_ =    intensity;
  color_ =        color;

  // Increment counter
  if (directional_)
  {
    directional_counters_[program_ID_]++;
  }
  else
  {
    point_light_counters_[program_ID_]++;
  }
  
  glUseProgram(program_ID_);

  light_position_ID_ =  glGetUniformLocation(program_ID_, "light_position");
  light_direction_ID_ = glGetUniformLocation(program_ID_, "light_direction");
  light_intensity_ID_ = glGetUniformLocation(program_ID_, "light_intensity");
  light_color_ID_ =     glGetUniformLocation(program_ID_, "light_color");
}

LightSource::~LightSource()
{
  // Increment counter
  if (directional_)
  {
    directional_counters_[program_ID_]--;
  }
  else
  {
    point_light_counters_[program_ID_]--;
  }
}

//! Sets the intensity of the light source.
/*!
  \param intensity is a positive value.
*/
void LightSource::setIntensity(float intensity)
{
  intensity_ = intensity;
}

//! Sets the color of the light source.
/*!
  \param color is a rgb vector with positive values.
*/
void LightSource::setColor(glm::vec3 color)
{
  color_ = color;
}

//! Render the LightSource.
/*!
  Setting all the uniforms of the lightsource for rendering.
  \param M is the transformation matrix of the parent.
*/
void LightSource::render(glm::mat4 M)
{
  Object3D::render(M * transform_matrix_);

  glUseProgram(program_ID_);  
  if (directional_)
  { // Directional, upload direction and not position
    glm::vec4 direction = M * transform_matrix_ * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
    glUniform3f(light_direction_ID_, direction.x, direction.y, direction.z);
  }
  else
  { // Not directional, upload position
    glm::vec4 position = M * transform_matrix_ * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glUniform3f(light_position_ID_, position.x, position.y, position.z);
  }

  glUniform1f(light_intensity_ID_, intensity_);
  glUniform3f(light_color_ID_, color_.r, color_.g, color_.b);
}

//! Static camera object
/*!
  Objects of type AbstractCameras subclasses should be added as children to
  this object 
*/
Object3D* SimpleGraphicsEngine::camera_;

//! Static camera object
/*!
  Objects of type AbstractCameras subclasses which are orthogonal should be
  added as children to this object. This camera renders objects in screen space.
*/
Object3D* SimpleGraphicsEngine::viewspace_ortho_camera_;

//! Constructor
/*!
  Create an object of SimpleGraphicsEngine.
  \param time is the currens global time.
*/
SimpleGraphicsEngine::SimpleGraphicsEngine(double time)
{
  time_ = time;
  dt_ = 0;
}

//! Destructor
SimpleGraphicsEngine::~SimpleGraphicsEngine()
{
  delete scene_;
  delete view_space_;
  delete background_space_;  
  
  delete camera_;
} 

//! Initializes OpenGL, creating context and adding all basic objects for the scene.
bool SimpleGraphicsEngine::initialize()
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
  
  scene_ = new Object3D();
  
  view_space_ = new Object3D();
  background_space_ = new Object3D();

  camera_ = new Object3D();
  viewspace_ortho_camera_ = new Object3D();

  scene_->addChild(camera_);  
  
  view_space_->addChild(viewspace_ortho_camera_);
  
  return true;
}

//! This function needs to be implemented if extending this class.
void SimpleGraphicsEngine::update()
{
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float aspect = float(window_width_) / window_height_;
  glViewport(0,0,window_width_ * 2, window_height_ * 2);

  glDisable(GL_DEPTH_TEST);
  background_space_->render(glm::mat4());
  glEnable(GL_DEPTH_TEST);
  scene_->render(glm::mat4());
  view_space_->render(glm::mat4());
}

//! Returns the current delta time which is set each time step
float SimpleGraphicsEngine::getDt()
{
  return dt_;
}

//! Returns the window width   
int SimpleGraphicsEngine::getWindowWidth()
{
  return window_width_;
}

//! Returns the window height
int SimpleGraphicsEngine::getWindowHeight()
{
  return window_height_;
}

//! Set resolution to new window width and height
/*!
  \param width is the new window width.
  \param height is the new window height.
*/
void SimpleGraphicsEngine::setWindowResolution(int width, int height)
{
  window_width_ = width;
  window_height_ = height;
}
