#include "elk/core/simple_forward_3d_renderer.h"

namespace elk { namespace core {

SimpleForward3DRenderer::SimpleForward3DRenderer(
  PerspectiveCamera& camera, int window_width, int window_height) :
  Renderer(camera, window_width, window_height)
{

}

SimpleForward3DRenderer::~SimpleForward3DRenderer()
{

}

void SimpleForward3DRenderer::render(Object3D& scene)
{
  // Submit all objects in the scene to the lists of renderable objects
  scene.submit(*this);

  glViewport(0,0, _window_width, _window_height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto renderable : _renderables_forward_to_render)
    renderable->render({ _camera });
  _renderables_forward_to_render.clear();  

  checkForErrors();
}

} }
