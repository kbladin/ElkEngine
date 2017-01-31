#include <gl/glew.h>
#include <gl/glfw3.h>

#include <SGE/core/simple_graphics_engine.h>
#include <SGE/window/application_window_glfw.h>
#include <SGE/core/shader_manager.h>
#include <SGE/core/new_mesh.h>
#include "sge/core/create_mesh.h"
#include "sge/core/create_texture.h"
#include "sge/core/texture_unit.h"
#include "sge/core/frame_buffer_object.h"
#include "sge/core/render_buffer_object.h"
#include "sge/core/renderer.h"
#include "sge/core/deferred_shading_renderer.h"
#include "sge/object_extensions/renderable_model.h"
#include "sge/object_extensions/framebuffer_quad.h"
#include "sge/object_extensions/renderable_grid.h"
#include "sge/object_extensions/light_source.h"

#include <functional>
#include <memory>

using namespace sge::core;
using namespace sge::window;

class MyEngine : public SimpleGraphicsEngine
{
public:
  MyEngine();
  ~MyEngine();

  void update();
  DeferredShadingRenderer& renderer() { return _renderer; };
private:
  DeferredShadingRenderer _renderer;
  RenderableModel _monkey;
  LightSource _lamp;
  LightSource _lamp2;
  LightSource _lamp3;
};

MyEngine::MyEngine() :
  SimpleGraphicsEngine(),
  _renderer(perspective_camera, 720 * 2, 480 * 2),
  _monkey("../../data/meshes/suzanne.obj"),
  _lamp(glm::vec3(1,1,1), 4),
  _lamp2(glm::vec3(0.5,0.5,1), 4),
  _lamp3(glm::vec3(1.0,0.5,1), 4)
{
  _lamp.setTransform(glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f)));
  _lamp2.setTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.0f, 2.0f)));
  _lamp3.setTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, 0.0f)));

  scene.addChild(_monkey);
  scene.addChild(_lamp);
  scene.addChild(_lamp2);
  scene.addChild(_lamp3);
}

MyEngine::~MyEngine()
{
  
}

void MyEngine::update()
{
  updateTransforms();

  _renderer.render(scene);
}

int main(int argc, char const *argv[])
{
  ApplicationWindowGLFW window(720, 480);
  MyEngine e;
  SphericalController controller(e.camera());
  WindowSizeController window_controller(e.renderer());
  window.addController(controller);
  window.addController(window_controller);
  std::function<void(void)> loop = [&]()
  {
    e.update();
  };
  
  window.run(loop);

  return 0;
}
