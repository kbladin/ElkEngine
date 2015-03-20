#include "ModelViewer.h"
#include "ModelLoader.h"
#include <iostream>
#include "Leap.h"

using namespace Leap;

SampleListener::SampleListener(ModelViewer* model_viewer)
{
  Listener::Listener();
  model_viewer_ = model_viewer;
}

void SampleListener::onConnect(const Controller& controller) {
  std::cout << "Leap motion Connected." << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {
  if (model_viewer_) {
    const Frame frame = controller.frame();
    std::cout << "Frame id: " << frame.id()
    << ", timestamp: " << frame.timestamp()
    << ", hands: " << frame.hands().count()
    << ", fingers: " << frame.fingers().count()
    << ", tools: " << frame.tools().count()
    << ", gestures: " << frame.gestures().count() << std::endl;
    
    Hand h = frame.hands()[0];
    
    model_viewer_->hand_->transform_.reset();
    model_viewer_->hand_->palm_mesh_->transform_.translate(glm::vec3(
                                                                     h.palmPosition().x,
                                                                     h.palmPosition().y,
                                                                     h.palmPosition().z));
  }
}

ModelViewer::ModelViewer() : SimpleGraphicsEngine(), listener_(this)
{
  controller_.addListener(listener_);
  
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<unsigned short> elements;
  
  bool loaded;
  do{
    // Load file
    std::cout << "Load file: ";
    std::string file_name = "../../data/testmodels/gargoyle.m";
    //std::cin >> file_name;
    loaded = ModelLoader::load(file_name.c_str(), &vertices, &normals, &elements);
  } while (!loaded);
  
  bunny_mesh_ = new TriangleMesh(vertices, normals, elements, program_ID_basic_render_);
  // "../../data/testmodels/bunny.m"
  // "../../data/testmodels/gargoyle.m"
  
  // Initialize all objects
  light_ = new LightSource(program_ID_basic_render_);
  bunny_ = new Object3D();
  bb_ = new BoundingBox(bunny_mesh_);
  hand_ = new HandObject3D(program_ID_basic_render_);

  // Change properties
  light_->transform_.translate(glm::vec3(10, 10, 0));
  light_->intensity_ = 100;

  camera_->transform_.translate(glm::vec3(0, -1, -3));
  camera_->transform_.rotateX(15);
  
  // Hierarchies
  bunny_mesh_->normalizeScale();
  bunny_->addChild(bunny_mesh_);

  //Add objects to scene
  scene_->addChild(bunny_);
  scene_->addChild(light_);
  //scene_->addChild(hand_);
  
  // Set callback functions
  glfwSetScrollCallback(window_, mouseScrollCallback);
}

ModelViewer::~ModelViewer()
{
  controller_.removeListener(listener_);
  
  delete bunny_;
  delete bunny_mesh_;
  delete light_;
  delete bb_;
  delete hand_;
}

void ModelViewer::update()
{
  SimpleGraphicsEngine::update();
}

void ModelViewer::mouseScrollCallback(GLFWwindow * window, double dx, double dy)
{
  // Transform back
  camera_->transform_.rotateX(-15);
  camera_->transform_.translate(glm::vec3(0, 1, 3));

  camera_->transform_.rotateX(- 5 * dy);
  
  float current_x_rotation = camera_->transform_.getEulerRotationXYZ().x;
  camera_->transform_.rotateX(-current_x_rotation);
  
  camera_->transform_.rotateY(- 5 * dx);

  camera_->transform_.rotateX(current_x_rotation);
  
  // Do transform again
  camera_->transform_.translate(glm::vec3(0, -1, -3));
  camera_->transform_.rotateX(15);
}

HandObject3D::HandObject3D(GLuint program_ID)
{
  palm_mesh_ = new TriangleMesh(program_ID);
  palm_mesh_->initBox(glm::vec3(0.5,0.1,0.5), -glm::vec3(0.5,0.1,0.5), glm::vec3(0,0,0));
  this->addChild(palm_mesh_);
  for (int i = 0; i < 5; i++) {
    finger_meshes_.push_back(new TriangleMesh(program_ID));
    finger_meshes_[i]->initBox(glm::vec3(0.1f), glm::vec3(-0.1f), glm::vec3(0,0,0));
    this->addChild(finger_meshes_[i]);
  }
}

HandObject3D::~HandObject3D()
{
  delete palm_mesh_;
  for (int i=0; i<finger_meshes_.size(); i++) {
    delete finger_meshes_[i];
  }
}