#include "sge/core/material.h"

#include "sge/core/create_texture.h"
#include "sge/core/texture_unit.h"

namespace sge { namespace core {

Material::Material(
  std::shared_ptr<Texture> albedo,
  std::shared_ptr<Texture> roughness,
  std::shared_ptr<Texture> IOR,
  std::shared_ptr<Texture> metalness)
{
  _albedo     = albedo    ? albedo    : CreateTexture::white(100,100);
  _roughness  = roughness ? roughness : CreateTexture::white(100,100);
  _IOR        = IOR       ? IOR       : CreateTexture::white(100,100);
  _metalness  = metalness ? metalness : CreateTexture::black(100,100);

  _albedo->upload();
  _roughness->upload();
  _IOR->upload();
  _metalness->upload();
}

Material::~Material()
{
  
}

void Material::use(GLuint programId)
{
  TextureUnit tex_unit_albedo, tex_unit_roughness, tex_unit_IOR, tex_unit_metalness;
  
  // Activate tex units and bind them to corresponding textures
  tex_unit_albedo.activate();
  _albedo->bind();
  tex_unit_roughness.activate();
  _roughness->bind();
  tex_unit_IOR.activate();
  _IOR->bind();
  tex_unit_metalness.activate();
  _metalness->bind();

  glUniform1i(glGetUniformLocation(programId, "albedo_texture"),    tex_unit_albedo);
  glUniform1i(glGetUniformLocation(programId, "roughness_texture"), tex_unit_roughness);
  glUniform1i(glGetUniformLocation(programId, "IOR_texture"),       tex_unit_IOR);
  glUniform1i(glGetUniformLocation(programId, "metalness_texture"), tex_unit_metalness);
}

} }
