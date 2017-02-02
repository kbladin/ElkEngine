#pragma once

#include "sge/core/texture.h"

#include <memory>

namespace sge { namespace core {

std::shared_ptr<Texture> loadTexture_freeimage(const char* path);
  
} }