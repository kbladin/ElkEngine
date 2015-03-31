#include "ModelViewer.h"
#include "ModelLoader.h"
#include <iostream>
#include "Leap.h"

using namespace Leap;

glm::mat4 leapMatrixToGlmMatrix(Matrix leap_M)
{
  double array[16];
  leap_M.toArray4x4(array);
  glm::mat4 M = glm::mat4();
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      M[i][j] = array[i * 4 + j];
    }
  }
  return M;
}

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
    /*
    std::cout << "Frame id: " << frame.id()
    << ", timestamp: " << frame.timestamp()
    << ", hands: " << frame.hands().count()
    << ", fingers: " << frame.fingers().count()
    << ", tools: " << frame.tools().count()
    << ", gestures: " << frame.gestures().count() << std::endl;
    */
    if (frame.hands().count() > 0 && frame.hands()[0].isRight() && model_viewer_->hand_) {
      Hand h = frame.hands()[0];
      
      glm::vec3 palm_position_camera_space = glm::vec3(h.palmPosition().x,
                                                      h.palmPosition().y,
                                                      h.palmPosition().z) /
      200.0f;
      
      // Update hand position
      glm::mat4 M = leapMatrixToGlmMatrix(h.basis());
      M = glm::translate(glm::mat4(), palm_position_camera_space + glm::vec3(0, - 1, 0) + model_viewer_->rotation_point) * M;
      M = glm::inverse(model_viewer_->camera_->transform_.getMatrix()) * M;
      model_viewer_->hand_->palm_mesh_->transform_.setBasis(M);
      
      FingerList fingers = h.fingers();
      for(int i = 0; i < fingers.count(); i++){
        Leap::Bone bone;
        Leap::Bone::Type bone_type;
        for(int b = 0; b < 4; b++)
        {
          bone_type = static_cast<Leap::Bone::Type>(b);
          bone = fingers[i].bone(bone_type);
          
          glm::vec3 bone_position_camera_space = glm::vec3(bone.center().x,
                                              bone.center().y,
                                              bone.center().z) /
                                              200.0f;
          glm::mat4 trans_M = glm::translate(glm::mat4(), bone_position_camera_space + glm::vec3(0, - 1, 0) + model_viewer_->rotation_point);
          

          glm::mat4 M = trans_M * leapMatrixToGlmMatrix(bone.basis());
          M = glm::inverse(model_viewer_->camera_->transform_.getMatrix()) * M;

          model_viewer_->hand_->fingers_[i]->bones_[b]->transform_.setBasis(M);
        }
      }
      
      float upperbound = 0.5;
      float lowerbound = 0.0;
      float diff_z = (palm_position_camera_space.z < upperbound) ? (palm_position_camera_space.z > lowerbound ? 0 : -(palm_position_camera_space.z - lowerbound)) : -(palm_position_camera_space.z - upperbound);
      
      upperbound = 1.5;
      lowerbound = 0.5;
      float diff_y = (palm_position_camera_space.y < upperbound) ? (palm_position_camera_space.y > lowerbound ? 0 : -(palm_position_camera_space.y - lowerbound)) : -(palm_position_camera_space.y - upperbound);
      
      upperbound = 0.5;
      lowerbound = -0.5;
      float diff_x = (palm_position_camera_space.x < upperbound) ? (palm_position_camera_space.x > lowerbound ? 0 : -(palm_position_camera_space.x - lowerbound)) : -(palm_position_camera_space.x - upperbound);
      
      
      glm::vec3 avg_finger_direction;
      for (int i=0; i<fingers.count(); i++) {
        avg_finger_direction += glm::vec3(fingers[i].direction().x, fingers[i].direction().y, fingers[i].direction().z);
      }
      avg_finger_direction = glm::normalize(avg_finger_direction);
      glm::vec3 hand_normal = glm::vec3(h.direction().x,h.direction().y,h.direction().z);
      
      if (glm::dot(avg_finger_direction, hand_normal) < 0.5 && h.palmNormal().y < 0)
      {
        glm::vec3 prev_position = model_viewer_->rotation_point;
        model_viewer_->camera_->transform_.translate(glm::vec3(0,0,-model_viewer_->rotation_point.z * diff_z * model_viewer_->dt_));
        
        model_viewer_->rotation_point.z *= (1 - diff_z*model_viewer_->dt_);
      
        // Transform back
        model_viewer_->camera_->transform_.translate(-prev_position);
      
        model_viewer_->camera_->transform_.rotateX(- 200 * diff_y * model_viewer_->dt_);
      
        float current_x_rotation = model_viewer_->camera_->transform_.getEulerRotationXYZ().x;
        model_viewer_->camera_->transform_.rotateX(-current_x_rotation);
      
        model_viewer_->camera_->transform_.rotateY(200 * diff_x * model_viewer_->dt_);
      
        model_viewer_->camera_->transform_.rotateX(current_x_rotation);
      
        // Do transform again
        model_viewer_->camera_->transform_.translate(prev_position);
      }
      else if (h.palmNormal().y < 0)
      {
        model_viewer_->camera_->transform_.translate(glm::vec3(0,0,0));
        model_viewer_->camera_->transform_.translate(glm::vec3(diff_x,diff_y,diff_z) * -model_viewer_->rotation_point.z * float(model_viewer_->dt_));
      }
      
      
      /*
      std::vector<Object3D*> colliding_children = model_viewer_->scene_->getCollidingChildren(glm::vec3(0.5,0.5,0.5));
      if (colliding_children.size() > 0) {
        std::cout << "KOLLIJOX!" << std::endl;
      }
*/
      
      
      for (int i=0; i<frame.gestures().count(); i++) {
        Gesture g = frame.gestures()[i];
        if (g.type() == Gesture::TYPE_KEY_TAP) {
          KeyTapGesture kt = KeyTapGesture(g);
          model_viewer_->hand_->tap_point_camera_space_ = glm::vec3(kt.position().x,kt.position().y, kt.position().z) / 200.0f;
          
          glm::vec3 point = glm::vec3(glm::inverse(model_viewer_->camera_->transform_.getMatrix()) * (glm::vec4(model_viewer_->hand_->tap_point_camera_space_,1) + glm::vec4(model_viewer_->rotation_point.x,-1 + model_viewer_->rotation_point.y,model_viewer_->rotation_point.z,0)));
          
          std::vector<Object3D*> colliding_children = model_viewer_->scene_->getCollidingChildren(point);
          if (colliding_children.size() > 0) {
            if (model_viewer_->selected_object_ == colliding_children[0]) {
              model_viewer_->selected_object_ = nullptr;
            }
            else
              model_viewer_->selected_object_ = colliding_children[0];
          }
          else
            model_viewer_->selected_object_ = nullptr;
          
          std::cout << "KEYTAP [" << point.x << " , " << point.y << " , " << point.z << "]" << std::endl;
        }
      }
      
      if (model_viewer_->selected_object_) {
        glm::vec3 scale = model_viewer_->selected_object_->transform_.getScale();
        //glm::vec3 eulerXYZ = model_viewer_->selected_object_->transform_.getEulerRotationXYZ();
        glm::vec3 position = model_viewer_->selected_object_->transform_.getPosition();
        model_viewer_->selected_object_->transform_.reset();
        model_viewer_->selected_object_->transform_.scale(scale);
        glm::vec3 hand_position = glm::vec3(h.palmPosition().x,h.palmPosition().y,h.palmPosition().z)/200.0f;
        
        model_viewer_->selected_object_->transform_.matrix_ = glm::translate(glm::mat4(), -glm::vec3(0.5,0.5,0.5)) * model_viewer_->selected_object_->transform_.matrix_;
  
        model_viewer_->selected_object_->transform_.matrix_ = leapMatrixToGlmMatrix(h.basis()) *  model_viewer_->selected_object_->transform_.matrix_;
        
        
        
        model_viewer_->selected_object_->transform_.translate(hand_position + glm::vec3(0, - 1, 0) + model_viewer_->rotation_point);
        
        //model_viewer_->selected_object_->transform_.rotateX(eulerXYZ.x);
        //model_viewer_->selected_object_->transform_.rotateY(eulerXYZ.y);
        //model_viewer_->selected_object_->transform_.rotateZ(eulerXYZ.z);
        
        model_viewer_->selected_object_->transform_.matrix_ = glm::inverse(model_viewer_->camera_->transform_.matrix_) * model_viewer_->selected_object_->transform_.getMatrix();
        
        if (h.palmNormal().y > 0) {
          model_viewer_->selected_object_->transform_.scale(glm::vec3( 1.0f + (h.palmPosition().y / 200.0f - 1.0f) * float(model_viewer_->dt_) * 1.0f));
        }
        
      }
    }
  }
}

ModelViewer::ModelViewer() : SimpleGraphicsEngine(), listener_(this)
{
  rotation_point = glm::vec3(0,0,-3);
  
  controller_.addListener(listener_);
  controller_.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
  
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<unsigned short> elements;
  
  bool loaded;
  do{
    // Load file
    std::cout << "Load file: ";
    std::string file_name = "../../data/testmodels/bunny.m";
    //std::cin >> file_name;
    loaded = ModelLoader::load(file_name.c_str(), &vertices, &normals, &elements);
  } while (!loaded);
  
  bunny_mesh_ = new TriangleMesh(vertices, normals, elements, program_ID_basic_render_);
  
  // "../../data/testmodels/bunny.m"
  // "../../data/testmodels/gargoyle.m"
  
  // Initialize all objects
  light_ = new LightSource(program_ID_basic_render_);
  light_mesh_ = new LightMesh3D(program_ID_one_color_shader_, 1);
  bunny_ = new Object3D();
  bb_ = new BoundingBox(bunny_mesh_);
  hand_ = new HandObject3D(program_ID_basic_render_);

  // Change properties
  light_->transform_.translate(glm::vec3(4, 4, 4));
  light_->intensity_ = 20;

  camera_->transform_.translate(rotation_point);
  
  // Hierarchies
  bunny_mesh_->normalizeScale();
  bunny_->addChild(bunny_mesh_);
  bunny_mesh_->addChild(bb_);
  
  light_->addChild(light_mesh_);

  //Add objects to scene
  scene_->addChild(bunny_);
  scene_->addChild(light_);
  scene_->addChild(hand_);
  
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
  delete light_mesh_;
}

void ModelViewer::update()
{
  SimpleGraphicsEngine::update();
}

void ModelViewer::mouseScrollCallback(GLFWwindow * window, double dx, double dy)
{
  
  glm::vec3 prev_position = camera_->transform_.getPosition();

  // Transform back
  camera_->transform_.translate(-prev_position);

  camera_->transform_.rotateX(- 5 * dy);
  
  float current_x_rotation = camera_->transform_.getEulerRotationXYZ().x;
  camera_->transform_.rotateX(-current_x_rotation);
  
  camera_->transform_.rotateY(- 5 * dx);

  camera_->transform_.rotateX(current_x_rotation);
  
  // Do transform again
  camera_->transform_.translate(prev_position);
}

FingerObject3D::FingerObject3D(GLuint program_ID)
{
  bones_.push_back(new TriangleMesh(program_ID));
  bones_[0]->initBox(glm::vec3(0.04f,0.04f,0.15f), -glm::vec3(0.04f,0.04f,0.15f), glm::vec3(0,0,0));
  
  bones_.push_back(new TriangleMesh(program_ID));
  bones_[1]->initBox(glm::vec3(0.04f,0.04f,0.1f), -glm::vec3(0.04f,0.04f,0.1f), glm::vec3(0,0,0));
  
  bones_.push_back(new TriangleMesh(program_ID));
  bones_[2]->initBox(glm::vec3(0.04f,0.04f,0.075f), -glm::vec3(0.04f,0.04f,0.075f), glm::vec3(0,0,0));
  
  bones_.push_back(new TriangleMesh(program_ID));
  bones_[3]->initBox(glm::vec3(0.04f,0.04f,0.05f), -glm::vec3(0.04f,0.04f,0.05f), glm::vec3(0,0,0));
}

FingerObject3D::~FingerObject3D()
{
  for (int i=0; i<bones_.size(); i++) {
    delete bones_[i];
  }
}

HandObject3D::HandObject3D(GLuint program_ID)
{
  //light_source_ = new LightSource(program_ID);
  //light_source_->intensity_ = 0.01;
  //light_source_->color_ = glm::vec3(0.1,1,0.1);
  palm_mesh_ = new TriangleMesh(program_ID);
  palm_mesh_->initBox(glm::vec3(0.15,0.05,0.15), -glm::vec3(0.15,0.05,0.15), glm::vec3(0,0,0));
  this->addChild(palm_mesh_);
  for (int i = 0; i < 5; i++) {
    fingers_.push_back(new FingerObject3D(program_ID));
    for (int j = 0; j < fingers_[i]->bones_.size(); j++) {
      this->addChild(fingers_[i]->bones_[j]);
    }
  }
  //fingers_[1]->bones_[3]->addChild(light_source_);
}

HandObject3D::~HandObject3D()
{
  delete palm_mesh_;
  //delete light_source_;
  for (int i = 0; i < 5; i++) {
    delete fingers_[i];
  }
}