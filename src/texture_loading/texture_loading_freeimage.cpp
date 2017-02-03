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
  FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(path,0);//Automatocally detects the format(from over 20 formats!)
  FIBITMAP* imagen = FreeImage_Load(formato, path);
 
  FIBITMAP* temp = imagen;
  imagen = FreeImage_ConvertTo32Bits(imagen);
  FreeImage_Unload(temp);
 
  int w = FreeImage_GetWidth(imagen);
  int h = FreeImage_GetHeight(imagen);
 
  GLubyte* textura = new GLubyte[4*w*h];
  char* pixeles = (char*)FreeImage_GetBits(imagen);

  //FreeImage loads in BGR format, so you need to swap some bytes(Or use GL_BGR). 
  for (int y = 0; y < h; ++y)
  {
    for (int x = 0; x < w; ++x)
    {
      int idx_read = x + (h - 1 - y) * w;
      int idx_write = x + y * w;

      textura[idx_write*4+0] = pixeles[idx_read*4+2];
      textura[idx_write*4+1] = pixeles[idx_read*4+1];
      textura[idx_write*4+2] = pixeles[idx_read*4+0];
      textura[idx_write*4+3] = pixeles[idx_read*4+3];
    }
  }
  
  return {textura, glm::uvec2(w,h)};
}

} }
