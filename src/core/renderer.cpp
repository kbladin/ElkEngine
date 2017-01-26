#include "SGE/core/renderer.h"

namespace sge { namespace core {

Renderer::Renderer(ShaderProgram& program) :
  _program(program)
{

}

Renderer::~Renderer()
{

}

void Renderer::render(Object3D& scene)
{
  _program.pushUsage();
  scene.execute();
  _program.popUsage();
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
