#pragma once

#include "elk/core/texture.h"
#include "elk/core/cube_map_texture.h"

#include <memory>

namespace elk { namespace core {

class CreateTexture
{
public:
  CreateTexture() {};
  ~CreateTexture() {};
  
  static std::shared_ptr<Texture> load(const char* path);
  static std::shared_ptr<CubeMapTexture> loadCubeMap(
    const char* path_positive_x, const char* path_negative_x,
    const char* path_positive_y, const char* path_negative_y,
    const char* path_positive_z, const char* path_negative_z);
  static std::shared_ptr<Texture> white(int width, int height);
  static std::shared_ptr<Texture> black(int width, int height);
private:
};

} }
