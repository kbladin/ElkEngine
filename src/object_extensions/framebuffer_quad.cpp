#include "sge/object_extensions/framebuffer_quad.h"
#include "sge/core/create_mesh.h"
#include "sge/core/create_texture.h"
#include "sge/core/texture_unit.h"
#include "sge/core/shader_program.h"

namespace sge { namespace core {

FrameBufferQuad::FrameBufferQuad(int width, int height, int n_color_attachments) :
  _width(width),
  _height(height),
  _depth_buffer(width, height)
{
  std::vector<GLuint> color_attachments;
  _quad = CreateMesh::quad();
  for (int i = 0; i < n_color_attachments; ++i)
  {
    _render_textures.push_back(
      std::make_shared<Texture>(glm::uvec3(width, height, 1), Texture::Format::RGBA,
        GL_RGBA, GL_FLOAT, Texture::FilterMode::Nearest,
        Texture::WrappingMode::ClampToEdge));
    color_attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
    
    _render_textures.back()->uploadTexture();
    _fbo.attach2DTexture(_render_textures.back()->id(), color_attachments.back(), 0);
  }
  _fbo.bind();
  glDrawBuffers(n_color_attachments, &color_attachments[0]);
  _fbo.unbind();
  _fbo.attachRenderBuffer(_depth_buffer.id(), GL_DEPTH_ATTACHMENT);
}

FrameBufferQuad::~FrameBufferQuad()
{

}

void FrameBufferQuad::execute()
{
  std::vector<TextureUnit> tex_units(_render_textures.size());
  for (int i = 0; i < _render_textures.size(); ++i)
  {
    tex_units[i].activate();
    _render_textures[i]->bind();
    const char* texure_uniform_name = (std::string("tex") + std::to_string(i)).c_str();
    glUniform1i(glGetUniformLocation(ShaderProgram::currentProgramId(), texure_uniform_name), tex_units[i]);
  }
  
  _quad->render();
}

} }
