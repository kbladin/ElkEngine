#include "sge/object_extensions/renderable_model.h"
#include "sge/core/create_mesh.h"
#include "sge/core/create_texture.h"
#include "sge/core/texture_unit.h"
#include "sge/core/shader_program.h"

namespace sge { namespace core {

RenderableModel::RenderableModel(const char* mesh_path)
{
  _new_mesh = CreateMesh::load(mesh_path);
  _tex = CreateTexture::white(100, 100);
  _tex->uploadTexture();
}

void RenderableModel::render()
{
  TextureUnit tex_unit;
  tex_unit.activate();
  _tex->bind();

  glUniform1i(glGetUniformLocation(ShaderProgram::currentProgramId(), "tex"), 0);

  glUniformMatrix4fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "M"),
    1,
    GL_FALSE,
    &absoluteTransform()[0][0]);

  _new_mesh->render();
}

} }
