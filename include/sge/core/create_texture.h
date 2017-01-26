#pragma once

#include "sge/core/texture.h"

#include <memory>

namespace sge { namespace core {

class CreateTexture
{
public:
  CreateTexture() {};
  ~CreateTexture() {};
  
  //static std::shared_ptr<Texture> load(const char* path);
  static std::shared_ptr<Texture> white(int width, int height);
private:
};

} }
