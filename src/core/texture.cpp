#include "sge/core/texture.h"
#include <cassert>
#include <cstring>

namespace sge { namespace core {

Texture::Texture(
  glm::uvec3 dimensions, Format format, GLint internalFormat, GLenum dataType,
  FilterMode filter, WrappingMode wrapping) :
  _dimensions(std::move(dimensions)),
  _format(format),
  _internal_format(internalFormat),
  _data_type(dataType),
  _filter(filter),
  _wrapping(wrapping),
  _mip_map_level(8),
  _anisotropy_level(-1.f),
  _has_ownership_of_data(false),
  _pixel_data(nullptr)
{
  initialize(true);
}

Texture::Texture(
  void* data, glm::uvec3 dimensions, Format format, GLint internalFormat,
  GLenum dataType, FilterMode filter, WrappingMode wrapping) :
  _dimensions(std::move(dimensions)),
  _format(format),
  _internal_format(internalFormat),
  _data_type(dataType),
  _filter(filter),
  _wrapping(wrapping),
  _mip_map_level(8),
  _anisotropy_level(-1.f),
  _has_ownership_of_data(true),
  _pixel_data(data)
{
  initialize(false);
}

Texture::~Texture()
{
  if (_id) {
    glDeleteTextures(1, &_id);
  }

  if (_has_ownership_of_data) {
    deallocateData();
  }
}

void Texture::initialize(bool allocate)
{
  determineTextureType();
  calculateBytesPerPixel();
  generate();
  if (allocate) {
    allocateData();
  }
  applyFilter();
  applyWrapping();
}

void Texture::allocateData()
{
  unsigned int array_size = _dimensions.x * _dimensions.y * _dimensions.z * _bytes_per_pixel;
  _pixel_data = new GLubyte[array_size];
  std::memset(_pixel_data, 0, array_size);
}

void Texture::deallocateData() {
  delete[] static_cast<GLubyte*>(_pixel_data);
  _pixel_data = nullptr;
}

void Texture::generate()
{
  _id = 0;
  glGenTextures(1, &_id);
}

void Texture::enable() const
{
  glEnable(_type);
}

void Texture::disable() const
{
  glDisable(_type);
}

void Texture::bind() const
{
  glBindTexture(_type, _id);
}

void Texture::applyFilter()
{
  bind();

  switch (_filter) {
    case FilterMode::Nearest:
      glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      break;
    case FilterMode::Linear:
      glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      break;
    case FilterMode::AnisotropicMipMap:
    {
      glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(_type, GL_TEXTURE_MAX_LEVEL, _mip_map_level - 1);
      glGenerateMipmap(_type);
      if (_anisotropy_level == -1.f) {
        GLfloat maxTextureAnisotropy = 1.0;
        glGetFloatv(
          GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,
          &maxTextureAnisotropy
        );
        _anisotropy_level = maxTextureAnisotropy;
      }
      glTexParameterf(_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, _anisotropy_level);
      glTexParameteri(_type, GL_TEXTURE_BASE_LEVEL, 0);
      break;
    }
    default:
      assert(false);
  }
}

int Texture::numberOfChannels() const
{
  return numberOfChannels(_format);
}

void Texture::applyWrapping()
{
  bind();
  GLint wrapping = GLint(_wrapping);
  switch (_type) {
    case GL_TEXTURE_3D:
      glTexParameteri(_type, GL_TEXTURE_WRAP_R, wrapping);
      // Intentional fallthrough
    case GL_TEXTURE_2D:
      glTexParameteri(_type, GL_TEXTURE_WRAP_T, wrapping);
      // Intentional fallthrough
    case GL_TEXTURE_1D:
      glTexParameteri(_type, GL_TEXTURE_WRAP_S, wrapping);
      break;
    default:
      assert(false);
  }
}

int Texture::numberOfChannels(Format format)
{
  switch (format)
  {
    case Format::Red:
    case Format::DepthComponent:
      return 1;
    case Format::RG:
      return 2;
    case Format::RGB:
      return 3;
    case Format::RGBA:
      return 4;
  }
  return 0;
}

void Texture::calculateBytesPerPixel()
{
  size_t num_channels = numberOfChannels();
  int sz_type = 0;
  switch (_data_type)
  {
    case GL_UNSIGNED_BYTE:
    case GL_BYTE:
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:
      sz_type = 1;
      break;
    case GL_UNSIGNED_SHORT:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
      sz_type = 2;
      break;
    case GL_UNSIGNED_INT:
    case GL_INT:
    case GL_FLOAT:
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
      sz_type = 4;
      break;
    default:
      assert(false);
  }
  _bytes_per_pixel = static_cast<GLubyte>(sz_type * num_channels);
}

void Texture::uploadTexture()
{
  bind();

  switch (_type) {
    case GL_TEXTURE_1D:
      glTexImage1D(
        _type,
        0,
        _internal_format,
        GLsizei(_dimensions.x),
        0,
        GLint(_format),
        _data_type,
        _pixel_data
      );
      break;
    case GL_TEXTURE_2D:
      glTexImage2D(
        _type,
        0,
        _internal_format,
        GLsizei(_dimensions.x),
        GLsizei(_dimensions.y),
        0,
        GLint(_format),
        _data_type,
        _pixel_data
      );
      break;
    case GL_TEXTURE_3D:
      glTexImage3D(
        _type,
        0,
        _internal_format,
        GLsizei(_dimensions.x),
        GLsizei(_dimensions.y),
        GLsizei(_dimensions.z),
        0,
        GLint(_format),
        _data_type,
        _pixel_data
      );
      break;
    default:
      assert(false);
  }
}

void Texture::downloadTexture()
{
  bind();
  if (!_pixel_data) {
    allocateData();
  }
  glGetTexImage(_type, 0, GLint(_format), _data_type, _pixel_data);
}

void Texture::determineTextureType()
{
  if (_dimensions.z == 1) {
    if (_dimensions.y == 1) {
      _type = GL_TEXTURE_1D;
    }
    else {
      _type = GL_TEXTURE_2D;
    }
  }
  else {
    _type = GL_TEXTURE_3D;
  }
}

} }
