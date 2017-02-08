#include "sge/texture_loading/texture_loading_freeimage.h"

#include <FreeImage.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>
#include <iostream>
#include <cassert>

namespace sge { namespace core {

std::pair<void*, glm::uvec2> loadTexture_freeimage(const char* path)
{
  FREE_IMAGE_FORMAT format = FreeImage_GetFileType(path,0);
  FIBITMAP* image = FreeImage_Load(format, path);
 
  FIBITMAP* temp = image;
  image = FreeImage_ConvertTo32Bits(image);
  FreeImage_Unload(temp);
 
  int w = FreeImage_GetWidth(image);
  int h = FreeImage_GetHeight(image);
 
  GLubyte* texture_data = new GLubyte[4*w*h];
  char* image_pixels = (char*)FreeImage_GetBits(image);

  //FreeImage loads in BGR format, swap the bytes. 
  for (int y = 0; y < h; ++y)
  {
    for (int x = 0; x < w; ++x)
    {
      int idx_read = x + y * w;
      int idx_write = x + y * w;

      texture_data[idx_write*4+0] = image_pixels[idx_read*4+2];
      texture_data[idx_write*4+1] = image_pixels[idx_read*4+1];
      texture_data[idx_write*4+2] = image_pixels[idx_read*4+0];
      texture_data[idx_write*4+3] = image_pixels[idx_read*4+3];
    }
  }
  FreeImage_Unload(image);
  return {texture_data, glm::uvec2(w,h)};
}

} }
