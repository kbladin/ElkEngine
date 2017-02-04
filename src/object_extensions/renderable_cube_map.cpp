#include "sge/object_extensions/renderable_cube_map.h"

#include "sge/core/create_mesh.h"
#include "sge/core/create_texture.h"
#include "sge/core/texture_unit.h"
#include "sge/core/shader_program.h"

namespace sge { namespace core {

RenderableCubeMap::RenderableCubeMap()
{
  _cube = CreateMesh::box(glm::vec3(-1.0f), glm::vec3(1.0f));
  _cube_map = CreateTexture::loadCubeMap(
    "../../data/textures/envmap_miramar/miramar_rt.tga",
    "../../data/textures/envmap_miramar/miramar_lf.tga",
    "../../data/textures/envmap_miramar/miramar_up.tga",
    "../../data/textures/envmap_miramar/miramar_dn.tga",
    "../../data/textures/envmap_miramar/miramar_bk.tga",
    "../../data/textures/envmap_miramar/miramar_ft.tga");
  _cube_map->upload();
}

RenderableCubeMap::~RenderableCubeMap()
{

}

void RenderableCubeMap::bindTexture()
{
  _cube_map->bind();
}

void RenderableCubeMap::render()
{
  TextureUnit tex_unit_cube_map;
  tex_unit_cube_map.activate();
  _cube_map->bind();
  glUniform1i(glGetUniformLocation(ShaderProgram::currentProgramId(), "cube_map"), tex_unit_cube_map);

  _cube->render();
}

} }
