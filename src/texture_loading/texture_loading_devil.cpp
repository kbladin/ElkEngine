#include "sge/texture_loading/texture_loading_devil.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>
#include <iostream>
#include <cassert>

namespace sge { namespace core {

std::shared_ptr<Texture> loadTexture_devil(const char* path)
{
/*
  unsigned int array_size = width * height * 4 * 1;
  GLubyte* pixel_data = new GLubyte[array_size];
  std::memset(pixel_data, 255, array_size);

  return std::make_shared<Texture>(pixel_data, glm::uvec3(width, height, 1));
  */
}

} }
