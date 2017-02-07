#include <gl/glew.h>
#include <gl/glfw3.h>

#include <SGE/core/simple_graphics_engine.h>
#include <SGE/window/application_window_glfw.h>
#include <SGE/core/shader_manager.h>
#include <SGE/core/mesh.h>
#include "sge/core/create_mesh.h"
#include "sge/core/create_texture.h"
#include "sge/core/texture_unit.h"
#include "sge/core/frame_buffer_object.h"
#include "sge/core/render_buffer_object.h"
#include "sge/core/renderer.h"
#include "sge/object_extensions/renderable_cube_map.h"
#include "sge/core/deferred_shading_renderer.h"
#include "sge/object_extensions/renderable_model.h"
#include "sge/object_extensions/framebuffer_quad.h"
#include "sge/object_extensions/renderable_grid.h"
#include "sge/object_extensions/light_source.h"
#include "sge/core/debug_input.h"

#include <functional>
#include <memory>

using namespace sge::core;
using namespace sge::window;

class MyEngine : public SimpleGraphicsEngine
{
public:
  MyEngine();
  ~MyEngine();

  void update(double dt);
  DeferredShadingRenderer& renderer() { return _renderer; };

  DeferredShadingRenderer _renderer;
private:
  RenderableModel _monkey;
  RenderableModel _earth;
  RenderableModel _gold_ball;
  RenderableModel _granite_ball;
  RenderableModel _greasy_metal_ball;
  RenderableModel _rusted_iron_ball;
  RenderableModel _worn_painted_ball;
  RenderableModel _plane;
  PointLightSource _lamp;
  DirectionalLightSource _lamp2;
};

MyEngine::MyEngine() :
  SimpleGraphicsEngine(),
  _renderer(perspective_camera, 720 * 2, 480 * 2),
  _monkey(CreateMesh::load("../../data/meshes/suzanne_highres.obj"),
    std::make_shared<Material>(
      CreateTexture::white(100,100),
      CreateTexture::load("../../data/textures/roughness.png"),
      nullptr,
      CreateTexture::white(100,100))),
  _earth(CreateMesh::lonLatSphere(64,32),
    std::make_shared<Material>(
      CreateTexture::load("../../data/textures/earth-albedo-highres.jpg"),
      CreateTexture::load("../../data/textures/earth-roughness-highres.png"))),
  _gold_ball(CreateMesh::lonLatSphere(64,32),
    std::make_shared<Material>(
      CreateTexture::load("../../data/textures/gold-scuffed-Unreal-Engine/gold-scuffed_basecolor.png"),
      CreateTexture::load("../../data/textures/gold-scuffed-Unreal-Engine/gold-scuffed_roughness.png"),
      CreateTexture::white(100,100),
      CreateTexture::load("../../data/textures/gold-scuffed-Unreal-Engine/gold-scuffed_metallic.png"),
      nullptr)),
  _granite_ball(CreateMesh::lonLatSphere(64,32),
    std::make_shared<Material>(
      CreateTexture::load("../../data/textures/granitesmooth1-Unreal-Engine/granitesmooth1-albedo.png"),
      CreateTexture::load("../../data/textures/granitesmooth1-Unreal-Engine/granitesmooth1-roughness3.png"),
      CreateTexture::white(100,100),
      CreateTexture::load("../../data/textures/granitesmooth1-Unreal-Engine/granitesmooth1-metalness.png"),
      nullptr)),
  _greasy_metal_ball(CreateMesh::lonLatSphere(64,32),
    std::make_shared<Material>(
      CreateTexture::load("../../data/textures/greasy-metal-pan1-Unreal-Engine/greasy-metal-pan1-albedo.png"),
      CreateTexture::load("../../data/textures/greasy-metal-pan1-Unreal-Engine/greasy-metal-pan1-roughness.png"),
      CreateTexture::white(100,100),
      CreateTexture::load("../../data/textures/greasy-metal-pan1-Unreal-Engine/greasy-metal-pan1-metal.png"),
      nullptr)),
  _rusted_iron_ball(CreateMesh::lonLatSphere(64,32),
    std::make_shared<Material>(
      CreateTexture::load("../../data/textures/rustediron1-alt2-Unreal-Engine/rustediron2_basecolor.png"),
      CreateTexture::load("../../data/textures/rustediron1-alt2-Unreal-Engine/rustediron2_roughness.png"),
      CreateTexture::white(100,100),
      CreateTexture::load("../../data/textures/rustediron1-alt2-Unreal-Engine/rustediron2_metallic.png"),
      nullptr)),
  _worn_painted_ball(CreateMesh::lonLatSphere(64,32),
    std::make_shared<Material>(
      CreateTexture::load("../../data/textures/wornpaintedcement-Unreal_Engine/wornpaintedcement-albedo.png"),
      CreateTexture::load("../../data/textures/wornpaintedcement-Unreal_Engine/wornpaintedcement-roughness.png"),
      CreateTexture::white(100,100),
      CreateTexture::load("../../data/textures/wornpaintedcement-Unreal_Engine/wornpaintedcement-metalness.png"),
      nullptr)),
  _plane(CreateMesh::quad(),
    std::make_shared<Material>(
      CreateTexture::white(100,100),
      CreateTexture::white(100,100))),
  _lamp(glm::vec3(1.0,0.8,0.6), 1.5),
  _lamp2(glm::vec3(1.0,0.8,0.7), 0.15)
{
  _renderer.setSkyBox(
    std::make_shared<RenderableCubeMap>(CreateTexture::loadCubeMap(
      "../../data/textures/mp_marvelous/bloody-marvelous_rt.tga",
      "../../data/textures/mp_marvelous/bloody-marvelous_lf.tga",
      "../../data/textures/mp_marvelous/bloody-marvelous_up.tga",
      "../../data/textures/mp_marvelous/bloody-marvelous_dn.tga",
      "../../data/textures/mp_marvelous/bloody-marvelous_bk.tga",
      "../../data/textures/mp_marvelous/bloody-marvelous_ft.tga")));

  _lamp.setTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
  _lamp2.setTransform(glm::rotate(float(M_PI) * 0.15f, glm::vec3(-1.0f, 0.0f, -1.0f)));
  _monkey.setTransform(glm::translate(glm::vec3(1.5f, 0.0f, 0.0f)));
  _earth.setTransform(glm::translate(glm::vec3(-1.5f, 0.0f, 0.0f)));
  _plane.setTransform(glm::scale(glm::vec3(4.0f, 4.0f, 4.0f)));
  _plane.setTransform(glm::rotate(-float(M_PI / 2), glm::vec3(1.0f, 0.0f, 0.0f)) * _plane.relativeTransform());
  _plane.setTransform(glm::translate(glm::vec3(0.0f, -1.0f, 0.0f)) * _plane.relativeTransform());
  

  _gold_ball.setTransform(glm::translate(glm::vec3(-4.0f, 0.0f, 0.0f)));
  _granite_ball.setTransform(glm::translate(glm::vec3(-2.0f, 0.0f, 0.0f)));
  _greasy_metal_ball.setTransform(glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)));
  _rusted_iron_ball.setTransform(glm::translate(glm::vec3(2.0f, 0.0f, 0.0f)));
  _worn_painted_ball.setTransform(glm::translate(glm::vec3(4.0f, 0.0f, 0.0f)));

  //scene.addChild(_monkey);
  //scene.addChild(_earth);
  
  scene.addChild(_gold_ball);
  scene.addChild(_granite_ball);
  scene.addChild(_greasy_metal_ball);
  scene.addChild(_rusted_iron_ball);
  scene.addChild(_worn_painted_ball);
  
  //scene.addChild(_plane);
  scene.addChild(camera());
  //camera().addChild(_lamp);
  scene.addChild(_lamp2);
}

MyEngine::~MyEngine()
{
  
}

void MyEngine::update(double dt)
{
  SimpleGraphicsEngine::update(dt);

  _renderer.render(scene);
}


class DebugInputController : public Controller
{
public:
  DebugInputController(MyEngine& engine);
  ~DebugInputController(){};

  virtual void step(float dt) override;
private:
  MyEngine& _engine;
};

DebugInputController::DebugInputController(MyEngine& engine) :
  _engine(engine)
{
  DebugInput::value("roughness") = 1.0;
  DebugInput::value("IOR") = 2.0;
  DebugInput::value("FOV") = 45.0;
}

void DebugInputController::step(float dt)
{
  if (_keys_pressed.count(Key::KEY_1))
  {
    _engine._renderer.setSkyBox(
      std::make_shared<RenderableCubeMap>(CreateTexture::loadCubeMap(
        "../../data/textures/Lycksele3/posx.jpg",
        "../../data/textures/Lycksele3/negx.jpg",
        "../../data/textures/Lycksele3/posy.jpg",
        "../../data/textures/Lycksele3/negy.jpg",
        "../../data/textures/Lycksele3/posz.jpg",
        "../../data/textures/Lycksele3/negz.jpg")));
  }
  else if (_keys_pressed.count(Key::KEY_2))
  {
    _engine._renderer.setSkyBox(
      std::make_shared<RenderableCubeMap>(CreateTexture::loadCubeMap(
        "../../data/textures/Yokohama2/posx.jpg",
        "../../data/textures/Yokohama2/negx.jpg",
        "../../data/textures/Yokohama2/posy.jpg",
        "../../data/textures/Yokohama2/negy.jpg",
        "../../data/textures/Yokohama2/posz.jpg",
        "../../data/textures/Yokohama2/negz.jpg")));
  }
  else if (_keys_pressed.count(Key::KEY_3))
  {
    _engine._renderer.setSkyBox(
      std::make_shared<RenderableCubeMap>(CreateTexture::loadCubeMap(
        "../../data/textures/Yokohama3/posx.jpg",
        "../../data/textures/Yokohama3/negx.jpg",
        "../../data/textures/Yokohama3/posy.jpg",
        "../../data/textures/Yokohama3/negy.jpg",
        "../../data/textures/Yokohama3/posz.jpg",
        "../../data/textures/Yokohama3/negz.jpg")));
  }
  else if (_keys_pressed.count(Key::KEY_4))
  {
    _engine._renderer.setSkyBox(
      std::make_shared<RenderableCubeMap>(CreateTexture::loadCubeMap(
        "../../data/textures/mp_marvelous/bloody-marvelous_rt.tga",
        "../../data/textures/mp_marvelous/bloody-marvelous_lf.tga",
        "../../data/textures/mp_marvelous/bloody-marvelous_up.tga",
        "../../data/textures/mp_marvelous/bloody-marvelous_dn.tga",
        "../../data/textures/mp_marvelous/bloody-marvelous_bk.tga",
        "../../data/textures/mp_marvelous/bloody-marvelous_ft.tga")));
  }
  else if (_keys_pressed.count(Key::KEY_6))
  {
    _engine._renderer.setSkyBox(
      std::make_shared<RenderableCubeMap>(CreateTexture::loadCubeMap(
        "../../data/textures/mp_alpha/alpha-island_rt.tga",
        "../../data/textures/mp_alpha/alpha-island_lf.tga",
        "../../data/textures/mp_alpha/alpha-island_up.tga",
        "../../data/textures/mp_alpha/alpha-island_dn.tga",
        "../../data/textures/mp_alpha/alpha-island_bk.tga",
        "../../data/textures/mp_alpha/alpha-island_ft.tga")));
  }

  if (_keys_pressed.count(Key::KEY_O))
  {
    DebugInput::value("roughness") *= (1.0 - dt * 5.0);
    std::cout << "roughness = " << DebugInput::value("roughness") << std::endl;
  }
  if (_keys_pressed.count(Key::KEY_P))
  {
    DebugInput::value("roughness") *= (1.0 + dt * 5.0);
    std::cout << "roughness = " << DebugInput::value("roughness") << std::endl;
  }
  
  if (_keys_pressed.count(Key::KEY_K))
  {
    DebugInput::value("IOR") *= (1.0 - dt);
    std::cout << "IOR = " << DebugInput::value("IOR") << std::endl;
  }
  if (_keys_pressed.count(Key::KEY_L))
  {
    DebugInput::value("IOR") *= (1.0 + dt);
    std::cout << "IOR = " << DebugInput::value("IOR") << std::endl;
  }

  if (_keys_pressed.count(Key::KEY_N))
  {
    DebugInput::value("FOV") += 0.001;
  }
  if (_keys_pressed.count(Key::KEY_M))
  {
    DebugInput::value("FOV") -= 0.001;
  }
  
  DebugInput::value("roughness") = glm::clamp(DebugInput::value("roughness"), 0.0005f, 50.0f);
  DebugInput::value("IOR") = glm::clamp(DebugInput::value("IOR"), 1.1f, 4.0f);

  _engine.camera().setFOV(DebugInput::value("FOV"));
}

int main(int argc, char const *argv[])
{
  ApplicationWindowGLFW window(720, 480);
  MyEngine e;
  
  // Controllers
  SphericalController controller(e.camera());
  WindowSizeController window_controller(e.renderer());
  DebugInputController debug_controller(e);
  
  // Add controllers
  window.addController(controller);
  window.addController(window_controller);
  window.addController(debug_controller);
  
  std::function<void(double)> loop = [&](double dt)
  {
    e.update(dt);
  };
  
  window.run(loop);

  return 0;
}
