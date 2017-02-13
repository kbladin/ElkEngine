#include "SGE/core/renderer.h"

namespace sge { namespace core {


Renderer::Renderer()
{ }

Renderer::~Renderer()
{ }

void Renderer::submitRenderableDeferred(RenderableDeferred& renderable)
{
  _renderables_deferred_to_render.push_back(&renderable);
}

void Renderer::submitRenderableForward(RenderableForward& renderable)
{
  _renderables_forward_to_render.push_back(&renderable);
}

void Renderer::submitPointLightSource(PointLightSource& light_source)
{
  _point_light_sources_to_render.push_back(&light_source);
}

void Renderer::submitDirectionalLightSource(DirectionalLightSource& light_source)
{
  _directional_light_sources_to_render.push_back(&light_source);
}

void Renderer::checkForErrors()
{
  GLenum error_code = glGetError();
  if (error_code != GL_NO_ERROR)
  {
    fprintf(stderr, "OpenGL ERROR : %s\n", gluErrorString(error_code));
  }
}

} }
