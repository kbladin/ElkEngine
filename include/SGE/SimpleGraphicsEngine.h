#ifndef SIMPLE_GRAPHICS_ENGINE_H
#define SIMPLE_GRAPHICS_ENGINE_H

#include "SGE/Object3D.h"
#include "SGE/Mesh.h"
#include "SGE/Camera.h"

#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <gl/glew.h>

//! This class manages all objects in the engine.
/*!
  This class has the scene which can be used to add more objects by adding
  children to the scene.
*/
class SimpleGraphicsEngine {
public:
  SimpleGraphicsEngine(int width, int height);
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

  PerspectiveCamera* camera;
  OrthoCamera* viewspace_ortho_camera;
private:
  virtual bool _initialize();

  // Data
  int _window_width;
  int _window_height;
};

#endif