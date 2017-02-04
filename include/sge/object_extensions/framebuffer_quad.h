#pragma once

#include "sge/core/object_3d.h"
#include "sge/core/mesh.h"
#include "sge/core/texture.h"
#include "sge/core/frame_buffer_object.h"
#include "sge/core/render_buffer_object.h"
#include "sge/core/texture_unit.h"

#include <vector>

namespace sge { namespace core {

class FrameBufferQuad : public Renderable
{
public:
  FrameBufferQuad(
    int width, int height, int n_color_attachments = 1);
  ~FrameBufferQuad();
  void bindTextures();
  void freeTextureUnits();
  virtual void render() override;
  inline void bindFBO() { _fbo.bind(); };
  inline void unbindFBO() { _fbo.unbind(); };
  inline int width() { return _width; };
  inline int height() { return _height; };
private:
  int _width, _height;
  std::shared_ptr<Mesh> _quad;
  FrameBufferObject _fbo;
  std::vector<std::shared_ptr<Texture> > _render_textures;
  RenderBufferObject _depth_buffer;
  std::vector<TextureUnit> _texture_units_in_use;
};

} }
