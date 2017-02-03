#include "sge/core/create_texture.h"

#include <glm/glm.hpp>
#include <vector>

#if SGE_USE_FREEIMAGE
#include "sge/texture_loading/texture_loading_freeimage.h"
#endif

namespace sge { namespace core {

std::shared_ptr<Texture> CreateTexture::load(const char* path)
{

#if SGE_USE_FREEIMAGE

auto texture_data = loadTexture_freeimage(path);

std::shared_ptr<Texture> tex = std::make_shared<Texture>(
  texture_data.first, glm::uvec3(texture_data.second,1),
  Texture::Format::RGBA, GL_RGBA, GL_UNSIGNED_BYTE,
  Texture::FilterMode::Linear, Texture::WrappingMode::Repeat);

return tex;
#else
printf("ERROR : No image library to load textures!");
return nullptr;
#endif
};

std::shared_ptr<CubeMapTexture> CreateTexture::loadCubeMap(
  const char* path_positive_x, const char* path_negative_x,
  const char* path_positive_y, const char* path_negative_y,
  const char* path_positive_z, const char* path_negative_z)
{
#if SGE_USE_FREEIMAGE
auto texture_data_positive_x = loadTexture_freeimage(path_positive_x);
auto texture_data_negative_x = loadTexture_freeimage(path_negative_x);
auto texture_data_positive_y = loadTexture_freeimage(path_positive_y);
auto texture_data_negative_y = loadTexture_freeimage(path_negative_y);
auto texture_data_positive_z = loadTexture_freeimage(path_positive_z);
auto texture_data_negative_z = loadTexture_freeimage(path_negative_z);

std::shared_ptr<CubeMapTexture> tex = std::make_shared<CubeMapTexture>(
  texture_data_positive_x.first, texture_data_negative_x.first,
  texture_data_positive_y.first, texture_data_negative_y.first,
  texture_data_positive_z.first, texture_data_negative_z.first,
  texture_data_positive_x.second.x,
  CubeMapTexture::Format::RGBA, GL_RGBA, GL_UNSIGNED_BYTE,
  CubeMapTexture::FilterMode::LinearMipMap, CubeMapTexture::WrappingMode::Repeat);

return tex;
#else
printf("ERROR : No image library to load textures!");
return nullptr;
#endif
}

std::shared_ptr<Texture> CreateTexture::white(int width, int height)
{
  unsigned int array_size = width * height * 4 * 1;
  GLubyte* pixel_data = new GLubyte[array_size];
  std::memset(pixel_data, 255, array_size);

  return std::make_shared<Texture>(pixel_data, glm::uvec3(width, height, 1));
}

std::shared_ptr<Texture> CreateTexture::black(int width, int height)
{
  unsigned int array_size = width * height * 4 * 1;
  GLubyte* pixel_data = new GLubyte[array_size];
  std::memset(pixel_data, 0, array_size);

  return std::make_shared<Texture>(pixel_data, glm::uvec3(width, height, 1));
}

} }
