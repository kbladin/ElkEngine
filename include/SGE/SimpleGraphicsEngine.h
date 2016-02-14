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
    GLuint    program_ID,
    float     intensity,
    glm::vec3 color);
  ~LightSource();
  virtual void render(glm::mat4 M);
  
  // Setters
  void setIntensity(float intensity);
  void setColor(glm::vec3 color);
private:
  // OpenGL handles
  GLuint _program_ID;
  GLuint _light_position_ID;
  GLuint _light_direction_ID;
  GLuint _light_intensityID;
  GLuint _light_colorID;

  // Data
  float     _intensity;
  glm::vec3 _color;
};

//! This class manages all objects in the engine.
/*!
  This class has the scene which can be used to add more objects by adding
  children to the scene.
*/
class SimpleGraphicsEngine {
public:
  SimpleGraphicsEngine();
  virtual ~SimpleGraphicsEngine();
  
  // Getters
  float getDt();
  int getWindowWidth();
  int getWindowHeight();

  // Setters
  virtual void setWindowResolution(int width, int height);
protected:
  void render();

  // Add children to these objects
  Object3D* scene;
  Object3D* view_space;
  Object3D* background_space;

  // Add cameras as children to these objects
  static Object3D* camera;
  static Object3D* viewspace_ortho_camera;
private:
  virtual bool _initialize();

  // Data
  int _window_width;
  int _window_height;
};

#endif