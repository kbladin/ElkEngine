#ifndef SIMPLE_GRAPHICS_ENGINE_H
#define SIMPLE_GRAPHICS_ENGINE_H

#include "SGE/ShaderManager.h"
#include "SGE/Object3D.h"
#include "SGE/Mesh.h"
#include "SGE/Camera.h"

#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <gl/glew.h>

//! A light source defined in 3D space
class LightSource : public Object3D {
public:
  LightSource(
    GLuint program_ID,
    float intensity,
    glm::vec3 color,
    bool directional);
  ~LightSource();
  virtual void render(glm::mat4 M);
  
  void setIntensity(float intensity);
  void setColor(glm::vec3 color);
private:
  const bool directional_;
  float intensity_;
  glm::vec3 color_;
  // Counters to keep track on the number of light sources
  // Each shader program has its own counter
  // Directional_counter keeps track of the number of directional light
  // sources while point_light_counter keeps track of the number of
  // point light sources.
  static std::map<GLuint, int> point_light_counters_;
  static std::map<GLuint, int> directional_counters_;

  GLuint program_ID_;
  GLuint light_position_ID_;
  GLuint light_direction_ID_;
  GLuint light_intensity_ID_;
  GLuint light_color_ID_;
};

//! This class manages all objects in the engine.
/*!
  This class has the scene_ which can be used to add more objects by adding
  children to the scene.
  The scene_ has some predefined children such as a grid plane and axes.
  Extend this class to create a program to run.
*/
class SimpleGraphicsEngine {
public:
  SimpleGraphicsEngine(double time);
  virtual ~SimpleGraphicsEngine();
  
  float getDt();
  int getWindowWidth();
  int getWindowHeight();

  virtual void setWindowResolution(int width, int height);
protected:
  void update();
  virtual bool initialize();

  double time_;  
  double dt_;
  int window_width_;
  int window_height_;

  Object3D* scene_;
  Object3D* view_space_;
  Object3D* background_space_;
  
  static Object3D* camera_;
  static Object3D* viewspace_ortho_camera_;
private:
};

#endif