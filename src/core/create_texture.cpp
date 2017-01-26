#include "sge/core/create_texture.h"

#include <glm/glm.hpp>
#include <vector>

namespace sge { namespace core {

  //std::shared_ptr<Texture> CreateTexture::load(const char* path)
  //{};
  
  std::shared_ptr<Texture> CreateTexture::white(int width, int height)
  {
    unsigned int array_size = width * height * 4 * 1;
    GLubyte* pixel_data = new GLubyte[array_size];
    std::memset(pixel_data, 255, array_size);

    return std::make_shared<Texture>(pixel_data, glm::uvec3(width, height, 1));
  }

} }
