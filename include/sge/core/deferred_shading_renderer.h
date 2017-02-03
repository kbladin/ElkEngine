#pragma once

#include "sge/core/object_3d.h"
#include "sge/core/camera.h"
#include "sge/core/shader_program.h"
#include "sge/core/renderer.h"
#include "sge/core/cube_map_texture.h"
#include "sge/object_extensions/framebuffer_quad.h"
#include "sge/object_extensions/renderable_cube_map.h"

#include <memory>
#include <vector>

namespace sge { namespace core {

class Renderable;
class PointLightSource;

class DeferredShadingRenderer {
public:
  DeferredShadingRenderer(
    PerspectiveCamera& camera, int framebuffer_width, int framebuffer_height);
  ~DeferredShadingRenderer();
  
  void submitRenderable(Renderable& renderable);
  void submitPointLightSource(PointLightSource& light_source);
  void submitDirectionalLightSource(DirectionalLightSource& light_source);

  void setWindowResolution(int width, int height);
  void render(Object3D& scene);
private:
  void renderGeometryBuffer(Object3D& scene);
  void renderPointLights();
  void renderDirectionalLights();
  void renderEnvironmentLights();
  void checkForErrors();

  int _window_width, _window_height;

  std::shared_ptr<ShaderProgram> _gbuffer_program;
  std::shared_ptr<ShaderProgram> _shading_program_point_lights;
  std::shared_ptr<ShaderProgram> _shading_program_directional_lights;
  std::shared_ptr<ShaderProgram> _shading_program_environment;
  std::shared_ptr<ShaderProgram> _cube_map_program;

  std::unique_ptr<FrameBufferQuad> _fbo_quad;
  std::unique_ptr<RenderableCubeMap> _cube_map;

  PerspectiveCamera& _camera;

  std::vector<Renderable*> _renderables_to_render;
  std::vector<PointLightSource*> _point_light_sources_to_render;
  std::vector<DirectionalLightSource*> _directional_light_sources_to_render;
};

} }
