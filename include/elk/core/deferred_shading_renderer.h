#pragma once

#include "elk/core/object_3d.h"
#include "elk/core/camera.h"
#include "elk/core/shader_program.h"
#include "elk/core/renderer.h"
#include "elk/core/cube_map_texture.h"
#include "elk/object_extensions/framebuffer_quad.h"
#include "elk/object_extensions/renderable_cube_map.h"

#include <memory>
#include <vector>

namespace elk { namespace core {

class Renderable;
class PointLightSource;
class DirectionalLightSource;

class DeferredShadingRenderer : public Renderer {
public:
  DeferredShadingRenderer(
    PerspectiveCamera& camera, int framebuffer_width, int framebuffer_height);
  ~DeferredShadingRenderer();
  
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
  
  // ping pong rendering if irradiance buffers
  void renderReflections(
    FrameBufferQuad& sample_buffer, FrameBufferQuad& output_buffer);
  void renderHighlights(
    FrameBufferQuad& sample_buffer, FrameBufferQuad& output_buffer);
  void renderPostProcess(
    FrameBufferQuad& sample_buffer, FrameBufferQuad& output_buffer);
  void renderPostProcessMotionBlur(
    FrameBufferQuad& sample_buffer, FrameBufferQuad& output_buffer);
  void forwardRenderIndependentRenderables(FrameBufferQuad& final_buffer);
  
  // Input is the fbo to sample from, index is the attachment to bind
  void renderToScreen(FrameBufferQuad& sample_fbo_quad, int attachment);

  // Internal render functions
  void renderPointLights();
  void renderDirectionalLights();
  void renderDiffuseEnvironmentLights();
  void renderSkyBox();
  void renderScreenSpaceReflections(FrameBufferQuad& sample_buffer);

  std::shared_ptr<ShaderProgram> _shading_program_point_lights;
  std::shared_ptr<ShaderProgram> _shading_program_directional_lights;
  std::shared_ptr<ShaderProgram> _shading_program_environment_diffuse;
  std::shared_ptr<ShaderProgram> _shading_program_reflections;
  std::shared_ptr<ShaderProgram> _shading_program_irradiance;
  std::shared_ptr<ShaderProgram> _cube_map_program;
  
  std::shared_ptr<ShaderProgram> _output_highlights_program;
  std::shared_ptr<ShaderProgram> _post_process_program;
  std::shared_ptr<ShaderProgram> _motion_blur_program;
  std::shared_ptr<ShaderProgram> _final_pass_through_program;

  std::unique_ptr<FrameBufferQuad> _geometry_fbo_quad;
  // Two irradiance FBOs to be able to perform ping ponging
  std::unique_ptr<FrameBufferQuad> _irradiance_fbo_quad1;
  std::unique_ptr<FrameBufferQuad> _irradiance_fbo_quad2;
  std::unique_ptr<FrameBufferQuad> _post_process_fbo_quad;

  std::shared_ptr<RenderableCubeMap> _sky_box;

  // Cached
  glm::mat4 _camera_previous_view_transform;
};

} }
