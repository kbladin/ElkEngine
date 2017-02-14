#pragma once

#include "elk/core/object_3d.h"
#include "elk/core/camera.h"
#include "elk/core/shader_program.h"

namespace elk { namespace core {

class RenderableDeferred;
class RenderableForward;
class PointLightSource;
class DirectionalLightSource;

class Renderer {
public:
  Renderer(PerspectiveCamera& camera, int window_width, int window_height);
  ~Renderer();
  
  void submitRenderableDeferred(RenderableDeferred& renderable);
  void submitRenderableForward(RenderableForward& renderable);
  void submitPointLightSource(PointLightSource& light_source);
  void submitDirectionalLightSource(DirectionalLightSource& light_source);

  void setWindowResolution(int width, int height);
  
  /**
	Should render all objects in the lists of renderables and light sources.
	When rendering is done, all lists need to be empty.
  */
  virtual void render(Object3D& scene) = 0;
protected:
  void checkForErrors();

  PerspectiveCamera& _camera;
  int _window_width, _window_height;

  std::vector<RenderableDeferred*> _renderables_deferred_to_render;
  std::vector<RenderableForward*> _renderables_forward_to_render;
  std::vector<PointLightSource*> _point_light_sources_to_render;
  std::vector<DirectionalLightSource*> _directional_light_sources_to_render;
};

} }
