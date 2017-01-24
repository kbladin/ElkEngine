#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>

namespace sge { namespace core {

class Texture
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
    AnisotropicMipMap
  };

  enum class WrappingMode {
    Repeat = GL_REPEAT,
    Clamp = GL_CLAMP,
    ClampToEdge = GL_CLAMP_TO_EDGE,
    ClampToBorder = GL_CLAMP_TO_BORDER,
    MirroredRepeat = GL_MIRRORED_REPEAT
  };

  static int numberOfChannels(Format format);
  
  Texture(glm::uvec3 dimensions, Format format = Format::RGBA,
          GLint internalFormat = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE,
          FilterMode filter = FilterMode::Linear,
          WrappingMode wrapping = WrappingMode::Repeat);

  Texture(void* data, glm::uvec3 dimensions, Format format = Format::RGBA,
          GLint internalFormat = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE,
          FilterMode filter = FilterMode::Linear,
          WrappingMode wrapping = WrappingMode::Repeat);

  ~Texture();

  void enable() const;
  void disable() const;

  void bind() const;

  int numberOfChannels() const;
  void uploadTexture();
  void downloadTexture();
  
protected:
  void initialize(bool allocate);

  void allocateData();
  void deallocateData();

  void generate();
  void applyFilter();
  void applyWrapping();
  void applySwizzleMask();
  void calculateBytesPerPixel();

  void determineTextureType();

private:
  glm::uvec3 _dimensions;

  Format _format;
  GLint _internal_format;
  GLenum _data_type;
  FilterMode _filter;
  WrappingMode _wrapping;

  GLuint _id;
  GLenum _type;
  GLubyte _bytes_per_pixel;

  int _mip_map_level;
  float _anisotropy_level;
  bool _has_ownership_of_data;

  void* _pixel_data;
};

} }
