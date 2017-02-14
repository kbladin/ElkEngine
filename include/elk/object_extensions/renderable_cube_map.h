#pragma once

#include "elk/core/object_3d.h"
#include "elk/core/mesh.h"
#include "elk/core/cube_map_texture.h"

namespace elk { namespace core {

class RenderableCubeMap
{
public:
  RenderableCubeMap(std::shared_ptr<CubeMapTexture> cube_map);
  ~RenderableCubeMap();
  
  void bindTexture();
  int textureSize();
  void render();
private:
  std::shared_ptr<Mesh> _cube;
  std::shared_ptr<CubeMapTexture> _cube_map;
};

} }



