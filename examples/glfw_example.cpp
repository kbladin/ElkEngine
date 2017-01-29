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
private:
  DeferredShadingRenderer _renderer;
  RenderableModel _monkey;
};

MyEngine::MyEngine() :
  SimpleGraphicsEngine(720, 480),
  _renderer(perspective_camera),
  _monkey("../../data/meshes/suzanne.obj")
{
  scene.addChild(_monkey);
}

MyEngine::~MyEngine()
{
  
}

void MyEngine::update()
{
  updateTransforms();

  _renderer.render(scene, windowWidth(), windowHeight());
}

int main(int argc, char const *argv[])
{
  ApplicationWindowGLFW window(720, 480);
  MyEngine e;
  SphericalController controller(e.camera());
  WindowSizeController window_controller(e);
  window.addController(controller);
  window.addController(window_controller);
  std::function<void(void)> loop = [&]()
  {
    e.update();
  };
  
  window.run(loop);

  return 0;
}
