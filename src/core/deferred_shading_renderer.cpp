#include "sge/core/deferred_shading_renderer.h"
#include "sge/core/shader_manager.h"

namespace sge { namespace core {

DeferredShadingRenderer::DeferredShadingRenderer(PerspectiveCamera& camera) :
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
    "shading_program",
    "../../shaders/deferred_shading/shading_pass.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/deferred_shading/shading_pass.frag");

  _gbuffer_program = ShaderManager::instance().getShader("gbuffer_program");
  _shading_program = ShaderManager::instance().getShader("shading_program");

  _gbuffer_renderer = std::make_unique<Renderer>(*_gbuffer_program);
  _shading_renderer = std::make_unique<Renderer>(*_shading_program);

  _camera.addToShader(_gbuffer_program->id());

  _fbo_quad = std::make_unique<FrameBufferQuad>(720 * 2, 480 * 2, 3);
}

DeferredShadingRenderer::~DeferredShadingRenderer()
{

}

void DeferredShadingRenderer::render(Object3D& scene, int width, int height)
{
  _fbo_quad->bindFBO();
  glViewport(0,0, _fbo_quad->width(), _fbo_quad->height());
  glClearColor(0.0, 0.0, 0.0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  _camera.execute();
  _gbuffer_renderer->render(scene);
  _fbo_quad->unbindFBO();

  glViewport(0,0, width, height);
  glDisable(GL_DEPTH_TEST);
  _shading_renderer->render(*_fbo_quad);

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
