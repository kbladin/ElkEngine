#include "elk/core/renderer.h"

namespace elk { namespace core {


Renderer::Renderer(PerspectiveCamera& camera, int window_width, int window_height) :
	_camera(camera),
	_window_width(window_width),
	_window_height(window_height)
{ }

Renderer::~Renderer()
{ }

void Renderer::setWindowResolution(int width, int height)
{
  _window_width = width;
  _window_height = height;
  _camera.setAspectRatio( static_cast<float>(width) / height);
}

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
