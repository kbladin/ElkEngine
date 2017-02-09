#pragma once

#include "sge/core/object_3d.h"
#include "sge/core/mesh.h"
#include "sge/core/texture.h"
#include "sge/core/frame_buffer_object.h"
#include "sge/core/render_buffer_object.h"
#include "sge/core/texture_unit.h"

#include <vector>
#include <memory>

namespace sge { namespace core {

class FrameBufferQuad : public Renderable
{
public:
  // Texture, attachment, name
  using RenderTexture = std::tuple<std::shared_ptr<Texture>, GLenum, std::string>;
  
  enum class UseDepthBuffer { YES, NO };

  FrameBufferQuad(
    int width, int height, std::vector<RenderTexture> render_textures,
    UseDepthBuffer depth_buffer = UseDepthBuffer::NO);
  ~FrameBufferQuad();
  void bindTextures();
  void generateMipMaps();
  void freeTextureUnits();
  virtual void render() override;
  void bindFBO();
  inline void unbindFBO() { _fbo.unbind(); };
  inline int width() { return _width; };
  inline int height() { return _height; };
private:

  int _width, _height;
  std::shared_ptr<Mesh> _quad;
  FrameBufferObject _fbo;
  std::vector<RenderTexture> _render_textures;
  std::unique_ptr<RenderBufferObject> _depth_buffer;
  std::vector<TextureUnit> _texture_units_in_use;

  // Cached color attachments
  std::vector<GLenum> _color_attachments;
};

} }
