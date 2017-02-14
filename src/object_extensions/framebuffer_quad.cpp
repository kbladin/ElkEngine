#include "elk/object_extensions/framebuffer_quad.h"
#include "elk/core/create_mesh.h"
#include "elk/core/create_texture.h"
#include "elk/core/texture_unit.h"
#include "elk/core/shader_program.h"

namespace elk { namespace core {

FrameBufferQuad::FrameBufferQuad(
  int width, int height, std::vector<RenderTexture> render_textures,
  UseDepthBuffer depth_buffer) :
  _width(width),
  _height(height),
  _render_textures(render_textures)
{
  _quad = CreateMesh::quad();
  
  for (auto render_texture : _render_textures)
  {
    auto texture = std::get<std::shared_ptr<Texture>>(render_texture);
    auto attachment = std::get<GLenum>(render_texture);
    
    _color_attachments.push_back(attachment);
    texture->upload();
    _fbo.attach2DTexture(texture->id(), attachment, 0);
  }

  if (depth_buffer == UseDepthBuffer::YES)
  {
    _depth_buffer = std::make_unique<RenderBufferObject>
      (width, height, GL_DEPTH_COMPONENT);
    _fbo.attachRenderBuffer(_depth_buffer->id(), GL_DEPTH_ATTACHMENT);
  }
}

FrameBufferQuad::~FrameBufferQuad()
{

}

void FrameBufferQuad::bindTextures()
{
  _texture_units_in_use = std::vector<TextureUnit>(_render_textures.size());
  
  for (int i = 0; i < _render_textures.size(); ++i)
  {
    auto texture = std::get<std::shared_ptr<Texture>>(_render_textures[i]);
    auto name = std::get<std::string>(_render_textures[i]);
    
    _texture_units_in_use[i].activate();
    texture->bind();
    glUniform1i(glGetUniformLocation(ShaderProgram::currentProgramId(),
      &name[0]), _texture_units_in_use[i]);
  }
}

void FrameBufferQuad::bindTextures(
  const std::vector<RenderTextureInfo>& render_texture_info)
{
  _texture_units_in_use = std::vector<TextureUnit>(render_texture_info.size());
  
  for (int i = 0; i < render_texture_info.size(); ++i)
  {
    int render_texture_index = std::get<int>(render_texture_info[i]);
    auto texture = std::get<std::shared_ptr<Texture>>(_render_textures[render_texture_index]);
    auto name = std::get<std::string>(render_texture_info[i]);
    
    _texture_units_in_use[i].activate();
    texture->bind();
    glUniform1i(glGetUniformLocation(ShaderProgram::currentProgramId(),
      &name[0]), _texture_units_in_use[i]);
  }
}

void FrameBufferQuad::bindFBO()
{
  _fbo.bind();
  // Prepair the buffers to draw to
  glDrawBuffers(_color_attachments.size(), &_color_attachments[0]);
}

void FrameBufferQuad::generateMipMaps()
{
  for (auto render_texture : _render_textures)
  {
    auto texture = std::get<std::shared_ptr<Texture>>(render_texture);
    texture->generateMipMap(); 
  }
}

void FrameBufferQuad::freeTextureUnits()
{
  _texture_units_in_use = std::vector<TextureUnit>();
}

void FrameBufferQuad::render()
{  
  _quad->render();
}

} }
