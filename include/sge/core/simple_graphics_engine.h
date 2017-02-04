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
  SimpleGraphicsEngine();
  virtual ~SimpleGraphicsEngine();
  
  // Getters
  PerspectiveCamera& camera();
  OrthoCamera& viewSpaceCamera();

protected:
  //! Update all objects
  void update(double dt);

  // Add children to these objects
  Object3D scene;
  Object3D view_space;
  Object3D background_space;

  PerspectiveCamera perspective_camera;
  OrthoCamera viewspace_ortho_camera;
private:
  //! Initializes GLEW, an OpenGL context needs to be active
  virtual bool _initializeGL();
};

} }
