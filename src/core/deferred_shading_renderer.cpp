#include "sge/core/deferred_shading_renderer.h"

#include "sge/core/shader_manager.h"
#include "sge/core/texture_unit.h"
#include "sge/core/create_texture.h"
#include "sge/object_extensions/light_source.h"

namespace sge { namespace core {

DeferredShadingRenderer::DeferredShadingRenderer(
  PerspectiveCamera& camera, int framebuffer_width, int framebuffer_height ) :
  _window_width(framebuffer_width),
  _window_height(framebuffer_height),
  _camera(camera)
{
  ShaderManager::instance().loadAndAddShader(
    "gbuffer_program",
    "../../shaders/deferred_shading/geometry_pass.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/deferred_shading/geometry_pass.frag");

  ShaderManager::instance().loadAndAddShader(
    "shading_program_point_lights",
    "../../shaders/deferred_shading/shading_pass.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/deferred_shading/shading_pass_point_light.frag");

  ShaderManager::instance().loadAndAddShader(
    "shading_program_directional_lights",
    "../../shaders/deferred_shading/shading_pass.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/deferred_shading/shading_pass_directional_light.frag");

  ShaderManager::instance().loadAndAddShader(
    "shading_program_environment_diffuse",
    "../../shaders/deferred_shading/shading_pass.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/deferred_shading/shading_pass_environment_diffuse.frag");

  ShaderManager::instance().loadAndAddShader(
    "shading_program_reflection",
    "../../shaders/deferred_shading/shading_pass.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/deferred_shading/shading_pass_reflection.frag");

  ShaderManager::instance().loadAndAddShader(
    "shading_program_irradiance",
    "../../shaders/deferred_shading/shading_pass.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/deferred_shading/shading_pass_irradiance.frag");

  ShaderManager::instance().loadAndAddShader(
    "cube_map_program",
    "../../shaders/deferred_shading/cube_map.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/deferred_shading/cube_map.frag");

  ShaderManager::instance().loadAndAddShader(
    "output_highlights_program",
    "../../shaders/deferred_shading/shading_pass.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/deferred_shading/shading_pass_output_highlights.frag");

  ShaderManager::instance().loadAndAddShader(
    "post_process_program",
    "../../shaders/deferred_shading/shading_pass.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/deferred_shading/shading_pass_post_process.frag");

  ShaderManager::instance().loadAndAddShader(
    "final_pass_through_program",
    "../../shaders/deferred_shading/shading_pass.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/deferred_shading/final_pass_through.frag");

  _gbuffer_program = ShaderManager::instance().getShader("gbuffer_program");
  _shading_program_point_lights =
    ShaderManager::instance().getShader("shading_program_point_lights");
  _shading_program_directional_lights =
    ShaderManager::instance().getShader("shading_program_directional_lights");
  _shading_program_environment_diffuse =
    ShaderManager::instance().getShader("shading_program_environment_diffuse");
  _shading_program_reflections =
    ShaderManager::instance().getShader("shading_program_reflection");
  _shading_program_irradiance =
    ShaderManager::instance().getShader("shading_program_irradiance");
  _cube_map_program = ShaderManager::instance().getShader("cube_map_program");
  _output_highlights_program = ShaderManager::instance().getShader("output_highlights_program");
  _post_process_program = ShaderManager::instance().getShader("post_process_program");
  _final_pass_through_program = ShaderManager::instance().getShader("final_pass_through_program");

  _camera.addToShader(_gbuffer_program->id());
  _camera.addToShader(_shading_program_point_lights->id());
  _camera.addToShader(_shading_program_directional_lights->id());
  _camera.addToShader(_cube_map_program->id());

  // Create render textures to be used in G-Buffer
  FrameBufferQuad::RenderTexture albedo_render_tex =
      {
        std::make_shared<Texture>(glm::uvec3(framebuffer_width, framebuffer_height, 1), Texture::Format::RGBA, GL_RGBA, GL_UNSIGNED_BYTE, Texture::FilterMode::Nearest,
          Texture::WrappingMode::ClampToEdge),
          GL_COLOR_ATTACHMENT0,
          "albedo_buffer"
      };
  FrameBufferQuad::RenderTexture position_render_tex =
      {
        std::make_shared<Texture>(glm::uvec3(framebuffer_width, framebuffer_height, 1), Texture::Format::RGB, GL_RGB16F, GL_HALF_FLOAT, Texture::FilterMode::Nearest,
          Texture::WrappingMode::ClampToEdge),
          GL_COLOR_ATTACHMENT1,
          "position_buffer"
      };
  FrameBufferQuad::RenderTexture normal_render_tex =
      {
        std::make_shared<Texture>(glm::uvec3(framebuffer_width, framebuffer_height, 1), Texture::Format::RGB, GL_RGB16F, GL_HALF_FLOAT, Texture::FilterMode::Nearest,
          Texture::WrappingMode::ClampToEdge),
          GL_COLOR_ATTACHMENT2,
          "normal_buffer"
      };
  FrameBufferQuad::RenderTexture material_render_tex =
      {
        std::make_shared<Texture>(glm::uvec3(framebuffer_width, framebuffer_height, 1), Texture::Format::RGB, GL_RGB16F, GL_HALF_FLOAT, Texture::FilterMode::Nearest,
          Texture::WrappingMode::ClampToEdge),
          GL_COLOR_ATTACHMENT3,
          "material_buffer"
      };
  
  // Create render textures to be used in Light-Buffer (irradiance before screen space reflections)
  FrameBufferQuad::RenderTexture irradiance_render_tex =
      {
        std::make_shared<Texture>(glm::uvec3(framebuffer_width, framebuffer_height, 1), Texture::Format::RGBA, GL_RGBA16F, GL_HALF_FLOAT, Texture::FilterMode::LinearMipMap,
          Texture::WrappingMode::ClampToEdge),
          GL_COLOR_ATTACHMENT0,
          "irradiance_buffer"
      };

  // Total irradiance after screen space reflections
  FrameBufferQuad::RenderTexture final_irradiance_render_tex =
    {
      std::make_shared<Texture>(glm::uvec3(framebuffer_width, framebuffer_height, 1), Texture::Format::RGB, GL_RGB16F, GL_HALF_FLOAT, Texture::FilterMode::LinearMipMap,
        Texture::WrappingMode::ClampToEdge),
        GL_COLOR_ATTACHMENT0,
        "final_irradiance_buffer"
    };


  FrameBufferQuad::RenderTexture bloom_render_tex =
    {
      std::make_shared<Texture>(glm::uvec3(framebuffer_width, framebuffer_height, 1), Texture::Format::RGB, GL_RGB16F, GL_HALF_FLOAT, Texture::FilterMode::Linear,
        Texture::WrappingMode::ClampToEdge),
        GL_COLOR_ATTACHMENT0,
        "bloom_buffer"
    };


  FrameBufferQuad::RenderTexture final_color_tex =
    {
      std::make_shared<Texture>(glm::uvec3(framebuffer_width, framebuffer_height, 1), Texture::Format::RGB, GL_RGB, GL_UNSIGNED_BYTE, Texture::FilterMode::Linear,
        Texture::WrappingMode::ClampToEdge),
        GL_COLOR_ATTACHMENT0,
        "color_buffer"
    };

  _geometry_fbo_quad = std::make_unique<FrameBufferQuad>(
    framebuffer_width, framebuffer_height,
    std::vector<FrameBufferQuad::RenderTexture>{
      albedo_render_tex,
      position_render_tex,
      normal_render_tex,
      material_render_tex });

  _light_fbo_quad = std::make_unique<FrameBufferQuad>(
    framebuffer_width, framebuffer_height,
    std::vector<FrameBufferQuad::RenderTexture>{irradiance_render_tex});

  _final_irradiance_fbo_quad = std::make_unique<FrameBufferQuad>(
    framebuffer_width, framebuffer_height,
    std::vector<FrameBufferQuad::RenderTexture>{
      final_irradiance_render_tex});

  _post_process_fbo_quad1 = std::make_unique<FrameBufferQuad>(
    framebuffer_width, framebuffer_height / 2,
    std::vector<FrameBufferQuad::RenderTexture>{
      bloom_render_tex});

  _final_pass_through_fbo_quad = std::make_unique<FrameBufferQuad>(
    framebuffer_width, framebuffer_height,
    std::vector<FrameBufferQuad::RenderTexture>{
      final_color_tex});
}

DeferredShadingRenderer::~DeferredShadingRenderer()
{

}

void DeferredShadingRenderer::setWindowResolution(int width, int height)
{
  _window_width = width;
  _window_height = height;
  _camera.setAspectRatio( static_cast<float>(width) / height);
}

void DeferredShadingRenderer::setSkyBox(std::shared_ptr<RenderableCubeMap> sky_box)
{
  _sky_box = sky_box;
}

void DeferredShadingRenderer::render(Object3D& scene)
{
  // Submit all objects in the scene to the lists of renderable objects
  scene.submit(*this);
  // Update cameras uniforms for all shaders
  _camera.execute();

  _geometry_fbo_quad->bindFBO();
  glViewport(0,0, _geometry_fbo_quad->width(), _geometry_fbo_quad->height());
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  // Render all GBuffers to the FBO-quads assigned textures
  renderGeometryBuffer(scene);
  _geometry_fbo_quad->unbindFBO();

  //_geometry_fbo_quad->generateMipMaps();

  _light_fbo_quad->bindFBO();
  // Setup for rendering light sources to the _light_fbo_quad frame buffer
  glViewport(0,0, _light_fbo_quad->width(), _light_fbo_quad->height());
  //glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_ONE, GL_ONE);
  glEnable(GL_BLEND);
  
  // Render light sources
  renderPointLights();
  renderDirectionalLights();

  if (_sky_box)
  {
    renderDiffuseEnvironmentLights();
    renderSkyBox();
  }
  _light_fbo_quad->unbindFBO();


  // Re-generate mip-map for irradiance texture
  _light_fbo_quad->generateMipMaps();

  _final_irradiance_fbo_quad->bindFBO();
  // Setup for rendering to the default FBO
  glViewport(0,0, _final_irradiance_fbo_quad->width(), _final_irradiance_fbo_quad->height());
  glDisable(GL_BLEND);
  
  if (_sky_box)
  {
    renderScreenSpaceReflections();
  }
  else
  {
    renderIrradiance();
  }
  _final_irradiance_fbo_quad->unbindFBO();



  _geometry_fbo_quad->freeTextureUnits();
  _light_fbo_quad->freeTextureUnits();


  // Re-generate mip-maps
  _final_irradiance_fbo_quad->generateMipMaps();


  _post_process_fbo_quad1->bindFBO();

  glViewport(0,0, _window_width, _window_height);

  _output_highlights_program->pushUsage();
  glUniform2i(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "window_size"),
    _post_process_fbo_quad1->width(), _post_process_fbo_quad1->height());
  _final_irradiance_fbo_quad->bindTextures();
  _final_irradiance_fbo_quad->render();
  _output_highlights_program->popUsage();

  _post_process_fbo_quad1->unbindFBO();




  _final_pass_through_fbo_quad->bindFBO();

  glViewport(0,0, _final_pass_through_fbo_quad->width(), _final_pass_through_fbo_quad->height());
  
  _post_process_program->pushUsage();
  glUniform2i(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "window_size"),
    _final_pass_through_fbo_quad->width(), _final_pass_through_fbo_quad->height());
  glUniform2i(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "bloom_buffer_base_size"),
    _post_process_fbo_quad1->width(), _post_process_fbo_quad1->height() * 2);
  _final_irradiance_fbo_quad->bindTextures();
  _post_process_fbo_quad1->bindTextures();
  _final_irradiance_fbo_quad->render();
  _post_process_program->popUsage();

  _final_pass_through_fbo_quad->unbindFBO();


  // Finally render to screen
  glViewport(0,0, _window_width, _window_height);
  _final_pass_through_program->pushUsage();
  glUniform2i(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "window_size"),
    _window_width, _window_height);
  _final_pass_through_fbo_quad->bindTextures();
  _final_pass_through_fbo_quad->render();
  _final_pass_through_program->popUsage();


  checkForErrors();
}

void DeferredShadingRenderer::renderGeometryBuffer(Object3D& scene)
{
  _gbuffer_program->pushUsage();
  
  for (auto it : _renderables_to_render)
  {
    it->render();
  }
  _renderables_to_render.clear();

  _gbuffer_program->popUsage();
}

void DeferredShadingRenderer::renderPointLights()
{
  _shading_program_point_lights->pushUsage();
  glUniform2i(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "window_size"),
    _light_fbo_quad->width(), _light_fbo_quad->height());

  glUniformMatrix4fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "P_frag"), 1, GL_FALSE,
    &_camera.projectionTransform()[0][0]);
  
  _geometry_fbo_quad->bindTextures();
  for (auto it : _point_light_sources_to_render)
  {
    it->render(_camera);
  }
  _point_light_sources_to_render.clear();
  _shading_program_point_lights->popUsage();
}

void DeferredShadingRenderer::renderDirectionalLights()
{
  _shading_program_directional_lights->pushUsage();
  glUniform2i(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "window_size"),
    _light_fbo_quad->width(), _light_fbo_quad->height());

  glUniformMatrix4fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "P_frag"), 1, GL_FALSE,
    &_camera.projectionTransform()[0][0]);
  _geometry_fbo_quad->bindTextures();
  for (auto it : _directional_light_sources_to_render)
  {
    it->render(_camera);
  }
  _directional_light_sources_to_render.clear();
  _shading_program_directional_lights->popUsage();
}

void DeferredShadingRenderer::renderDiffuseEnvironmentLights()
{
  _shading_program_environment_diffuse->pushUsage();
  glUniform2i(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "window_size"),
    _light_fbo_quad->width(), _light_fbo_quad->height());

  glm::mat3 V_inv = glm::mat3(_camera.absoluteTransform());
  glUniformMatrix3fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "V_inv"),
    1,
    GL_FALSE,
    &V_inv[0][0]);

  glUniformMatrix4fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "P_frag"), 1, GL_FALSE,
    &_camera.projectionTransform()[0][0]);

  _geometry_fbo_quad->bindTextures();
  _sky_box->render();

  _shading_program_environment_diffuse->popUsage();
}

void DeferredShadingRenderer::renderSkyBox()
{
  _cube_map_program->pushUsage();
  glUniform2i(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "window_size"),
    _light_fbo_quad->width(), _light_fbo_quad->height());
  _geometry_fbo_quad->bindTextures();
    glDisable(GL_CULL_FACE);
  _sky_box->render();
  glEnable(GL_CULL_FACE);
  _cube_map_program->popUsage();
}

void DeferredShadingRenderer::renderScreenSpaceReflections()
{
  _shading_program_reflections->pushUsage();
  glUniform2i(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "window_size"),
    _final_irradiance_fbo_quad->width(), _final_irradiance_fbo_quad->height());

  glUniformMatrix4fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "P_frag"), 1, GL_FALSE,
    &_camera.projectionTransform()[0][0]);

  glm::mat3 V_inv = glm::mat3(_camera.absoluteTransform());
  glUniformMatrix3fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "V_inv"),
    1,
    GL_FALSE,
    &V_inv[0][0]);

  _light_fbo_quad->bindTextures();
  _geometry_fbo_quad->bindTextures();
  _sky_box->render();
  _shading_program_reflections->popUsage();
}

void DeferredShadingRenderer::renderIrradiance()
{
  _shading_program_irradiance->pushUsage();
  glUniform2i(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "window_size"),
    _final_irradiance_fbo_quad->width(), _final_irradiance_fbo_quad->height());
  _light_fbo_quad->bindTextures();
  _light_fbo_quad->render();
  _shading_program_irradiance->popUsage();
}


} }
