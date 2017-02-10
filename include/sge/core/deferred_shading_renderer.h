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
  // Initialization. Called from constructor
  void initializeShaders();
  void initializeFramebuffers(int framebuffer_width, int framebuffer_height);

  // External render functions called from render()
  // Input is the fbo to render to
  void renderGeometryBuffer(FrameBufferQuad& geometry_buffer);
  void renderLightSources(FrameBufferQuad& light_buffer);
  void renderReflections(FrameBufferQuad& irradiance_buffer);
  void renderHighlights(FrameBufferQuad& post_process_buffer);
  void renderPostProcess(FrameBufferQuad& final_buffer);
  void forwardRenderIndependentRenderables(FrameBufferQuad& final_buffer);
  
  // Input is the fbo to sample from, index is the attachment to bind
  void renderToScreen(FrameBufferQuad& sample_fbo_quad, int attachment);

  // Internal render functions
  void renderPointLights();
  void renderDirectionalLights();
  void renderDiffuseEnvironmentLights();
  void renderSkyBox();
  void renderScreenSpaceReflections();
  void renderIrradiance();

  int _window_width, _window_height;

  std::shared_ptr<ShaderProgram> _gbuffer_program;
  std::shared_ptr<ShaderProgram> _shading_program_point_lights;
  std::shared_ptr<ShaderProgram> _shading_program_directional_lights;
  std::shared_ptr<ShaderProgram> _shading_program_environment_diffuse;
  std::shared_ptr<ShaderProgram> _shading_program_reflections;
  std::shared_ptr<ShaderProgram> _shading_program_irradiance;
  std::shared_ptr<ShaderProgram> _cube_map_program;
  
  std::shared_ptr<ShaderProgram> _output_highlights_program;
  std::shared_ptr<ShaderProgram> _post_process_program;
  std::shared_ptr<ShaderProgram> _final_pass_through_program;

  std::unique_ptr<FrameBufferQuad> _geometry_fbo_quad;
  std::unique_ptr<FrameBufferQuad> _light_fbo_quad;
  std::unique_ptr<FrameBufferQuad> _final_irradiance_fbo_quad;
  std::unique_ptr<FrameBufferQuad> _post_process_fbo_quad;
  std::unique_ptr<FrameBufferQuad> _final_pass_through_fbo_quad;

  std::shared_ptr<RenderableCubeMap> _sky_box;

  PerspectiveCamera& _camera;
};

} }
