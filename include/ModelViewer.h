#ifndef MODEL_VIEWER_H
#define MODEL_VIEWER_H

#include "SimpleGraphicsEngine.h"

#include "Leap.h"

using namespace Leap;

class ModelViewer;

class SampleListener : public Listener {
public:
  //SampleListener();
  SampleListener(ModelViewer* model_viewer);
  virtual void onConnect(const Controller&);
  virtual void onFrame(const Controller&);
private:
  ModelViewer* model_viewer_;
};

class HandObject3D;

class FingerObject3D : public Object3D {
public:
  FingerObject3D(GLuint program_ID);
  ~FingerObject3D();
private:
  friend SampleListener;
  friend HandObject3D;
  std::vector<TriangleMesh*> bones_;
};

class HandObject3D : public Object3D {
public:
  HandObject3D(GLuint program_ID);
  ~HandObject3D();

private:
  friend SampleListener;
  TriangleMesh* palm_mesh_;
  std::vector<FingerObject3D*> fingers_;
};

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
  friend SampleListener;
  // Leap motion objects
  Controller controller_;
  SampleListener listener_;
  // Objects to put in the scene
  Object3D* bunny_;
  TriangleMesh* bunny_mesh_;
  LightSource* light_;
  BoundingBox* bb_;
  HandObject3D* hand_;
  
  Object3D* camera_pivot_;
};

#endif