#include "sge/core/cube_map_texture.h"
#include <cassert>
#include <cstring>

namespace sge { namespace core {

CubeMapTexture::CubeMapTexture(
  int side, Format format, GLint internalFormat, GLenum dataType,
  FilterMode filter, WrappingMode wrapping) :
  _side(side),
  _format(format),
  _internal_format(internalFormat),
  _data_type(dataType),
  _filter(filter),
  _wrapping(wrapping),
  _type(GL_TEXTURE_CUBE_MAP),
  _mip_map_level(8),
  _anisotropy_level(-1.f),
  _has_ownership_of_data(false),
  _pixel_data_positive_x(nullptr),
  _pixel_data_negative_x(nullptr),
  _pixel_data_positive_y(nullptr),
  _pixel_data_negative_y(nullptr),
  _pixel_data_positive_z(nullptr),
  _pixel_data_negative_z(nullptr)
{
  initialize(true);
}

CubeMapTexture::CubeMapTexture(
  void* data_positive_x, void* data_negative_x, void* data_positive_y,
  void* data_negative_y, void* data_positive_z, void* data_negative_z,
  int side, Format format, GLint internalFormat, GLenum dataType,
  FilterMode filter, WrappingMode wrapping) :
  _side(side),
  _format(format),
  _internal_format(internalFormat),
  _data_type(dataType),
  _filter(filter),
  _wrapping(wrapping),
  _type(GL_TEXTURE_CUBE_MAP),
  _mip_map_level(8),
  _anisotropy_level(-1.f),
  _has_ownership_of_data(true),
  _pixel_data_positive_x(data_positive_x),
  _pixel_data_negative_x(data_negative_x),
  _pixel_data_positive_y(data_positive_y),
  _pixel_data_negative_y(data_negative_y),
  _pixel_data_positive_z(data_positive_z),
  _pixel_data_negative_z(data_negative_z)
{
  initialize(false);
}

CubeMapTexture::~CubeMapTexture()
{
  if (_id) {
    glDeleteTextures(1, &_id);
  }

  if (_has_ownership_of_data) {
    deallocateData();
  }
}

void CubeMapTexture::initialize(bool allocate)
{
  calculateBytesPerPixel();
  generate();
  if (allocate) {
    allocateData();
  }
  applyFilter();
  applyWrapping();
}

void CubeMapTexture::allocateData()
{
  unsigned int array_size = _side * _side * _bytes_per_pixel;
  _pixel_data_positive_x = new GLubyte[array_size];
  _pixel_data_negative_x = new GLubyte[array_size];
  _pixel_data_positive_y = new GLubyte[array_size];
  _pixel_data_negative_y = new GLubyte[array_size];
  _pixel_data_positive_z = new GLubyte[array_size];
  _pixel_data_negative_z = new GLubyte[array_size];
  std::memset(_pixel_data_positive_x, 0, array_size);
  std::memset(_pixel_data_negative_x, 0, array_size);
  std::memset(_pixel_data_positive_y, 0, array_size);
  std::memset(_pixel_data_negative_y, 0, array_size);
  std::memset(_pixel_data_positive_z, 0, array_size);
  std::memset(_pixel_data_negative_z, 0, array_size);
}

void CubeMapTexture::deallocateData() {
  delete[] static_cast<GLubyte*>(_pixel_data_positive_x);
  delete[] static_cast<GLubyte*>(_pixel_data_negative_x);
  delete[] static_cast<GLubyte*>(_pixel_data_positive_y);
  delete[] static_cast<GLubyte*>(_pixel_data_negative_y);
  delete[] static_cast<GLubyte*>(_pixel_data_positive_z);
  delete[] static_cast<GLubyte*>(_pixel_data_negative_z);
  _pixel_data_positive_x = nullptr;
  _pixel_data_negative_x = nullptr;
  _pixel_data_positive_y = nullptr;
  _pixel_data_negative_y = nullptr;
  _pixel_data_positive_z = nullptr;
  _pixel_data_negative_z = nullptr;
}

void CubeMapTexture::generate()
{
  _id = 0;
  glGenTextures(1, &_id);
}

void CubeMapTexture::enable() const
{
  glEnable(_type);
}

void CubeMapTexture::disable() const
{
  glDisable(_type);
}

void CubeMapTexture::bind() const
{
  glBindTexture(_type, _id);
}

void CubeMapTexture::applyFilter()
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
    case FilterMode::LinearMipMap:
      glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      upload();
      glGenerateMipmap(_type);
      glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
      break;
    case FilterMode::AnisotropicMipMap:
    {
      glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(_type, GL_TEXTURE_MAX_LEVEL, _mip_map_level - 1);
      upload();
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

int CubeMapTexture::numberOfChannels() const
{
  return numberOfChannels(_format);
}

void CubeMapTexture::applyWrapping()
{
  bind();
  GLint wrapping = GLint(_wrapping);
  glTexParameteri(_type, GL_TEXTURE_WRAP_R, wrapping);
  glTexParameteri(_type, GL_TEXTURE_WRAP_S, wrapping);
  glTexParameteri(_type, GL_TEXTURE_WRAP_T, wrapping);
}

int CubeMapTexture::numberOfChannels(Format format)
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

void CubeMapTexture::calculateBytesPerPixel()
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

void CubeMapTexture::upload()
{
  bind();

  glTexImage2D(
    GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, _internal_format, _side, _side, 0,
    GLint(_format), _data_type, _pixel_data_positive_x);
  glTexImage2D(
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, _internal_format, _side, _side, 0,
    GLint(_format), _data_type, _pixel_data_negative_x);
  glTexImage2D(
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, _internal_format, _side, _side, 0,
    GLint(_format), _data_type, _pixel_data_positive_y);
  glTexImage2D(
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, _internal_format, _side, _side, 0,
    GLint(_format), _data_type, _pixel_data_negative_y);
  glTexImage2D(
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, _internal_format, _side, _side, 0,
    GLint(_format), _data_type, _pixel_data_positive_z);
  glTexImage2D(
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, _internal_format, _side, _side, 0,
    GLint(_format), _data_type, _pixel_data_negative_z);
}

} }
