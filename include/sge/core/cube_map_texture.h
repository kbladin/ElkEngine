#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>

namespace sge { namespace core {

class CubeMapTexture
{
public:
  enum class Format {
    Red = GL_RED,
    RG = GL_RG,
    RGB = GL_RGB,
    RGBA = GL_RGBA,
    DepthComponent = GL_DEPTH_COMPONENT
  };

  enum class FilterMode {
    Nearest,
    Linear,
    LinearMipMap,
    AnisotropicMipMap
  };

  enum class WrappingMode {
    Repeat = GL_REPEAT,
    ClampToEdge = GL_CLAMP_TO_EDGE,
    ClampToBorder = GL_CLAMP_TO_BORDER,
    MirroredRepeat = GL_MIRRORED_REPEAT
  };

  static int numberOfChannels(Format format);
  
  CubeMapTexture(
    int side, Format format = Format::RGBA, GLint internalFormat = GL_RGBA,
    GLenum dataType = GL_UNSIGNED_BYTE, FilterMode filter = FilterMode::Linear,
    WrappingMode wrapping = WrappingMode::Repeat);

  CubeMapTexture(
    void* data_positive_x, void* data_negative_x, void* data_positive_y,
    void* data_negative_y, void* data_positive_z, void* data_negative_z,
    int side, Format format = Format::RGBA, GLint internalFormat = GL_RGBA,
    GLenum dataType = GL_UNSIGNED_BYTE, FilterMode filter = FilterMode::Linear,
    WrappingMode wrapping = WrappingMode::Repeat);

  ~CubeMapTexture();

  void enable() const;
  void disable() const;

  void bind() const;

  int numberOfChannels() const;
  inline int size() { return _side; } const;
  void upload();

  inline GLuint id() const {return _id;};
  
protected:
  void initialize(bool allocate);

  void allocateData();
  void deallocateData();

  void generate();
  void applyFilter();
  void applyWrapping();
  void applySwizzleMask();
  void calculateBytesPerPixel();

private:
  int _side;

  Format _format;
  GLint _internal_format;
  GLenum _data_type;
  FilterMode _filter;
  WrappingMode _wrapping;

  GLuint _id;
  const GLenum _type;
  GLubyte _bytes_per_pixel;

  int _mip_map_level;
  float _anisotropy_level;
  bool _has_ownership_of_data;

  void* _pixel_data_positive_x;
  void* _pixel_data_negative_x;
  void* _pixel_data_positive_y;
  void* _pixel_data_negative_y;
  void* _pixel_data_positive_z;
  void* _pixel_data_negative_z;
};

} }
