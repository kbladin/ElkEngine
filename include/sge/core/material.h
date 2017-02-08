#pragma once

#include "sge/core/texture.h"

#include <memory>

namespace sge { namespace core {

class Material
{
public:
  Material(
    std::shared_ptr<Texture> albedo     = nullptr,
    std::shared_ptr<Texture> roughness  = nullptr,
    std::shared_ptr<Texture> R0        = nullptr,
    std::shared_ptr<Texture> metalness  = nullptr,
    std::shared_ptr<Texture> normal     = nullptr);
  ~Material();

  void use(GLuint programId);
  
private:
  std::shared_ptr<Texture> _albedo;
  std::shared_ptr<Texture> _roughness;
  std::shared_ptr<Texture> _R0;
  std::shared_ptr<Texture> _metalness;
  std::shared_ptr<Texture> _normal;
};

} }