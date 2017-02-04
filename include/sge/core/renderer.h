#pragma once

#include "SGE/core/object_3d.h"
#include "SGE/core/shader_program.h"

namespace sge { namespace core {

class Renderable;
class PointLightSource;
class DirectionalLightSource;

class Renderer {
public:
  Renderer();
  ~Renderer();
  
  void submitRenderable(Renderable& renderable);
  void submitPointLightSource(PointLightSource& light_source);
  void submitDirectionalLightSource(DirectionalLightSource& light_source);

  /**
	Should render all objects in the lists of renderables and light sources.
	When rendering is done, all lists need to be empty.
  */
  virtual void render(Object3D& scene) = 0;
protected:
  void checkForErrors();

  std::vector<Renderable*> _renderables_to_render;
  std::vector<PointLightSource*> _point_light_sources_to_render;
  std::vector<DirectionalLightSource*> _directional_light_sources_to_render;
};

} }
