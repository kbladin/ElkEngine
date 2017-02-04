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
    "shading_program_environment",
    "../../shaders/deferred_shading/shading_pass.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/deferred_shading/shading_pass_environment.frag");

  ShaderManager::instance().loadAndAddShader(
    "cube_map_program",
    "../../shaders/deferred_shading/cube_map.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/deferred_shading/cube_map.frag");

  _gbuffer_program = ShaderManager::instance().getShader("gbuffer_program");
  _shading_program_point_lights =
    ShaderManager::instance().getShader("shading_program_point_lights");
  _shading_program_directional_lights =
    ShaderManager::instance().getShader("shading_program_directional_lights");
  _shading_program_environment =
    ShaderManager::instance().getShader("shading_program_environment");
  _cube_map_program = ShaderManager::instance().getShader("cube_map_program");

  _camera.addToShader(_gbuffer_program->id());
  _camera.addToShader(_shading_program_point_lights->id());
  _camera.addToShader(_shading_program_directional_lights->id());
  _camera.addToShader(_cube_map_program->id());

  _fbo_quad = std::make_unique<FrameBufferQuad>(_window_width, _window_height, 4);
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

  _fbo_quad->bindFBO();
  glViewport(0,0, _fbo_quad->width(), _fbo_quad->height());
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  // Render all GBuffers to the FBO-quads assigned textures
  renderGeometryBuffer(scene);
  _fbo_quad->unbindFBO();

  // Setup for rendering light sources to the default frame buffer
  glViewport(0,0, _window_width, _window_height);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
  glEnable(GL_BLEND);
  
  // Render light sources
  renderPointLights();
  renderDirectionalLights();

  if (_sky_box)
  {
    renderEnvironmentLights();
    renderSkyBox();
  }

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
    _window_width, _window_height);

  glUniformMatrix4fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "P_frag"), 1, GL_FALSE,
    &_camera.projectionTransform()[0][0]);
  
  _fbo_quad->bindTextures();
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
    _window_width, _window_height);

  glUniformMatrix4fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "P_frag"), 1, GL_FALSE,
    &_camera.projectionTransform()[0][0]);
  _fbo_quad->bindTextures();
  for (auto it : _directional_light_sources_to_render)
  {
    it->render(_camera);
  }
  _directional_light_sources_to_render.clear();
  _shading_program_directional_lights->popUsage();
}

void DeferredShadingRenderer::renderEnvironmentLights()
{
  _shading_program_environment->pushUsage();
  glUniform2i(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "window_size"),
    _window_width, _window_height);

  glm::mat3 V_inv = glm::mat3(_camera.absoluteTransform());
  glUniformMatrix3fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "V_inv"),
    1,
    GL_FALSE,
    &V_inv[0][0]);

  glUniformMatrix4fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "P_frag"), 1, GL_FALSE,
    &_camera.projectionTransform()[0][0]);

  _fbo_quad->bindTextures();
  _sky_box->render();

  _shading_program_environment->popUsage();
}

void DeferredShadingRenderer::renderSkyBox()
{
  _cube_map_program->pushUsage();
  glUniform2i(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "window_size"),
    _window_width, _window_height);
  _fbo_quad->bindTextures();
  glDisable(GL_CULL_FACE);
  _sky_box->render();
  glEnable(GL_CULL_FACE);
  _cube_map_program->popUsage();
}

} }
