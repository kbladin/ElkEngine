#include "sge/core/deferred_shading_renderer.h"

#include "sge/core/shader_manager.h"
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
    "shading_program_directional_light",
    "../../shaders/deferred_shading/shading_pass.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/deferred_shading/shading_pass.frag");

  _gbuffer_program = ShaderManager::instance().getShader("gbuffer_program");
  _shading_program_point_lights = ShaderManager::instance().getShader("shading_program_point_lights");
  _shading_program_directional_light = ShaderManager::instance().getShader("shading_program_directional_light");

  _camera.addToShader(_gbuffer_program->id());
  _camera.addToShader(_shading_program_point_lights->id());

  _fbo_quad = std::make_unique<FrameBufferQuad>(_window_width, _window_height, 4);
}

DeferredShadingRenderer::~DeferredShadingRenderer()
{

}

void DeferredShadingRenderer::submitRenderable(Renderable& renderable)
{
  _renderables_to_render.push_back(&renderable);
}

void DeferredShadingRenderer::submitLightSource(LightSource& light_source)
{
  _light_sources_to_render.push_back(&light_source);
}

void DeferredShadingRenderer::setWindowResolution(int width, int height)
{
  _window_width = width;
  _window_height = height;
  _camera.setAspectRatio( static_cast<float>(width) / height);
}

void DeferredShadingRenderer::render(Object3D& scene)
{

  scene.submit(*this);

  _fbo_quad->bindFBO();
  glViewport(0,0, _fbo_quad->width(), _fbo_quad->height());
  glClearColor(0.0, 0.0, 0.0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  _camera.execute();
  _gbuffer_program->pushUsage();
  
  for (auto it : _renderables_to_render)
  {
    it->render();
  }
  _renderables_to_render.clear();

  _gbuffer_program->popUsage();
  _fbo_quad->unbindFBO();

  glViewport(0,0, _window_width, _window_height);
  glClearColor(0.0, 0.0, 0.0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  // Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
  
  _shading_program_point_lights->pushUsage();
  
  glUniform2i(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "window_size"),
    _window_width, _window_height);
  _fbo_quad->bindTextures();
  for (auto it : _light_sources_to_render)
  {
    it->render(_camera);
  }
  _light_sources_to_render.clear();
  _shading_program_point_lights->popUsage();
  
  _shading_program_directional_light->pushUsage();
  
  glUniform2i(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "window_size"),
    _window_width, _window_height);
  _fbo_quad->bindTextures();
  //_fbo_quad->render();
  _shading_program_directional_light->popUsage();
  
  checkForErrors();
}

void DeferredShadingRenderer::checkForErrors()
{
  GLenum error_code = glGetError();
  if (error_code != GL_NO_ERROR)
  {
    fprintf(stderr, "OpenGL ERROR : %s\n", gluErrorString(error_code));
  }
}

} }
