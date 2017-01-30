#pragma once

#include "sge/core/object_3d.h"
#include "sge/core/camera.h"
#include "sge/core/shader_program.h"
#include "sge/core/renderer.h"
#include "sge/object_extensions/framebuffer_quad.h"

#include <memory>

namespace sge { namespace core {

class DeferredShadingRenderer {
public:
  DeferredShadingRenderer(
    PerspectiveCamera& camera, int framebuffer_width, int framebuffer_height);
  ~DeferredShadingRenderer();
  
  void setWindowResolution(int width, int height);
  void render(Object3D& scene);
private:
  void checkForErrors();

  int _window_width, _window_height;

  std::shared_ptr<ShaderProgram> _gbuffer_program;
  std::shared_ptr<ShaderProgram> _shading_program;

  std::unique_ptr<Renderer> _gbuffer_renderer;
  std::unique_ptr<Renderer> _shading_renderer;

  std::unique_ptr<FrameBufferQuad> _fbo_quad;

  PerspectiveCamera& _camera;
};

} }
