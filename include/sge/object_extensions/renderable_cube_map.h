#pragma once

#include "sge/core/object_3d.h"
#include "sge/core/mesh.h"
#include "sge/core/cube_map_texture.h"

namespace sge { namespace core {

class RenderableCubeMap : public Renderable
{
public:
  RenderableCubeMap(std::shared_ptr<CubeMapTexture> cube_map);
  ~RenderableCubeMap();
  
  void bindTexture();
  virtual void render() override;
private:
  std::shared_ptr<Mesh> _cube;
  std::shared_ptr<CubeMapTexture> _cube_map;
};

} }



