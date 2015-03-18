#include "ModelViewer.h"

ModelViewer::ModelViewer() : SimpleGraphicsEngine()
{
  // Initialize all objects
  light_ = new LightSource(program_ID_basic_render_);
  bunny_ = new Object3D();
  bunny_mesh_ = new Mesh("../../data/testmodels/bunny.m", program_ID_basic_render_);
  bb_ = new BoundingBox(bunny_mesh_);

  // Change properties
  light_->transform_.translate(glm::vec3(10, 10, 0));
  light_->intensity_ = 100;

  camera_->transform_.translate(glm::vec3(0, -1, -3));
  camera_->transform_.rotate(15, glm::vec3(1,0,0));
  
  // Hierarchies
  bunny_mesh_->normalizeScale();
  bunny_->addChild(bunny_mesh_);

  //Add objects to scene
  scene_->addChild(bunny_);
  scene_->addChild(light_);
}

ModelViewer::~ModelViewer()
{
  delete bunny_;
  delete bunny_mesh_;
  delete light_;
  delete bb_;
}

void ModelViewer::update()
{
  SimpleGraphicsEngine::update();
  
  // Transform back
  camera_->transform_.rotate(-15, glm::vec3(1,0,0));
  camera_->transform_.translate(glm::vec3(0, 1, 3));
  
  camera_->transform_.rotate(50 * dt_, glm::vec3(0,1,0));

  // Do transform again
  camera_->transform_.translate(glm::vec3(0, -1, -3));
  camera_->transform_.rotate(15, glm::vec3(1,0,0));
}