#ifndef MODEL_VIEWER_H
#define MODEL_VIEWER_H

#include "SimpleGraphicsEngine.h"

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