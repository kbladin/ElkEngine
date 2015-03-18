#ifndef MODEL_VIEWER_H
#define MODEL_VIEWER_H

#include "SimpleGraphicsEngine.h"

class ModelViewer : public SimpleGraphicsEngine {
public:
  ModelViewer();
  ~ModelViewer();
  virtual void update();
  
private:
  // Objects to put in the scene
  Object3D* bunny_;
  Mesh* bunny_mesh_;
  LightSource* light_;
  BoundingBox* bb_;
};

#endif