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
class DirectionalLightSource;

class DeferredShadingRenderer : public Renderer {
public:
  DeferredShadingRenderer(
    PerspectiveCamera& camera, int framebuffer_width, int framebuffer_height);
  ~DeferredShadingRenderer();
  
  void setWindowResolution(int width, int height);
  void setSkyBox(std::shared_ptr<RenderableCubeMap> sky_box);
  virtual void render(Object3D& scene) override;
private:
  void renderGeometryBuffer(Object3D& scene);
  void renderPointLights();
  void renderDirectionalLights();
  void renderEnvironmentLights();
  void renderSkyBox();

  int _window_width, _window_height;

  std::shared_ptr<ShaderProgram> _gbuffer_program;
  std::shared_ptr<ShaderProgram> _shading_program_point_lights;
  std::shared_ptr<ShaderProgram> _shading_program_directional_lights;
  std::shared_ptr<ShaderProgram> _shading_program_environment;
  std::shared_ptr<ShaderProgram> _cube_map_program;

  std::shared_ptr<RenderableCubeMap> _sky_box;

  std::unique_ptr<FrameBufferQuad> _fbo_quad;

  PerspectiveCamera& _camera;
};

} }
