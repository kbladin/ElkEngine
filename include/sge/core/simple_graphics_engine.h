#pragma once

#include "SGE/core/object_3d.h"
#include "SGE/core/mesh.h"
#include "SGE/core/camera.h"

#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <gl/glew.h>

namespace sge { namespace core {

//! This class manages all objects in the engine.
/*!
  This class has the scene which can be used to add more objects by adding
  children to it.
*/
class SimpleGraphicsEngine {
public:
  SimpleGraphicsEngine(int width, int height);
  virtual ~SimpleGraphicsEngine();
  
  // Getters
  int windowWidth();
  int windowHeight();
  PerspectiveCamera& camera();
  OrthoCamera& viewSpaceCamera();

  // Setters
  virtual void setWindowResolution(int width, int height);
protected:
  //! Update the transforms of all objects in the scene and renders them
  void render();

  // Add children to these objects
  Object3D scene;
  Object3D view_space;
  Object3D background_space;

  PerspectiveCamera perspective_camera;
  OrthoCamera viewspace_ortho_camera;
private:
  //! Initializes GLEW, an OpenGL context needs to be active
  virtual bool _initializeGL();
  void checkForErrors();

  // Data
  int _window_width;
  int _window_height;
};

} }