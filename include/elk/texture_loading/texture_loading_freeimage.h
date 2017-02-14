#pragma once

#include "elk/core/texture.h"

#include <memory>

namespace elk { namespace core {

std::pair<void*, glm::uvec2> loadTexture_freeimage(const char* path);
  
} }