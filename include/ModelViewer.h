#ifndef MODEL_VIEWER_H
#define MODEL_VIEWER_H

#include "SimpleGraphicsEngine.h"

//! The actual program extending SimpleGraphicsEngine
/*!
 Here, all objects can be added to the scene_ and input is handled outside of
 SimpleGraphicsEngine.
*/
class ModelViewer : public SimpleGraphicsEngine {
public:
  ModelViewer();
  ~ModelViewer();
  virtual void update();
  static void mouseScrollCallback(GLFWwindow * window, double dx, double dy);
private:
  // Objects to put in the scene
  Object3D* bunny_;
  TriangleMesh* bunny_mesh_;
  LightSource* light_;
  BoundingBox* bb_;
};

#endif