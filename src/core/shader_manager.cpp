#include "sge/core/shader_manager.h"
#include "sge/core/file_utils.h"


#include <array>
#include <vector>

namespace sge { namespace core {

ShaderManager ShaderManager::_instance;

ShaderManager::ShaderManager()
{

}

ShaderManager::~ShaderManager()
{

}

ShaderManager& ShaderManager::instance()
{
  return _instance;
}

void ShaderManager::loadAndAddShader(
    std::string name,
    const char* vs_src,
    const char* tcs_src,
    const char* tes_src,
    const char* gs_src,
    const char* fs_src)
{
  _shader_programs.insert(std::make_pair(name, std::make_unique<ShaderProgram>(
    name, vs_src, tcs_src, tes_src, gs_src, fs_src)));
}

ShaderProgram* ShaderManager::getShader(std::string name)
{
  return _shader_programs[name].get();
}

} }
