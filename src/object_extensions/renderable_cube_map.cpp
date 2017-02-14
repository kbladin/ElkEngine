#include "elk/object_extensions/renderable_cube_map.h"

#include "elk/core/create_mesh.h"
#include "elk/core/texture_unit.h"
#include "elk/core/shader_program.h"
#include "elk/core/camera.h"

namespace elk { namespace core {

RenderableCubeMap::RenderableCubeMap(std::shared_ptr<CubeMapTexture> cube_map) :
  _cube_map(cube_map)
{
  _cube = CreateMesh::box(glm::vec3(-1.0f), glm::vec3(1.0f));
  _cube_map->upload();
}

RenderableCubeMap::~RenderableCubeMap()
{

}

void RenderableCubeMap::bindTexture()
{
  _cube_map->bind();
}

int RenderableCubeMap::textureSize()
{
	return _cube_map->size();
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
