#pragma once

#include "sge/core/texture.h"

#include <memory>

namespace sge { namespace core {

std::pair<void*, glm::uvec2> loadTexture_freeimage(const char* path);
  
} }