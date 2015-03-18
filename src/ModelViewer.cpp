#include "ModelViewer.h"
#include "ModelLoader.h"
#include <iostream>

ModelViewer::ModelViewer() : SimpleGraphicsEngine()
{
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
  
  // Set callback functions
  glfwSetScrollCallback(window_, mouseScrollCallback);
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