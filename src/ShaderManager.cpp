#include "SGE/ShaderManager.h"

ShaderManager* ShaderManager::instance_;

ShaderManager::ShaderManager()
{

}

//! Destructor
ShaderManager::~ShaderManager()
{
  for (std::map<std::string, GLuint>::const_iterator it =
  		shader_program_IDs.begin();
  	it != shader_program_IDs.end();
  	it++) {
    glDeleteProgram(it->second);
  }
}

//! Returning the instance that can be used for calling other functions.
/*!
  This function can not be called before an OpenGL context is created.
*/
ShaderManager* ShaderManager::instance()
{
  if (!instance_) {
    instance_ = new ShaderManager();
  }
  return instance_;
}

void ShaderManager::loadShader(
  std::string name,
  const char* vs_src,
  const char* tcs_src,
  const char* tes_src,
  const char* gs_src,
  const char* fs_src)
{
  shader_program_IDs.insert(
    std::pair<std::string,GLuint>(name,
    ShaderLoader::loadShaders(
      vs_src, // Vertex shader file path
      tcs_src, // Tesselation control shader file path
      tes_src, // Tesselation evaluation shader file path
      gs_src, // Geometry shader file path
      fs_src))); // Fragment shader file path
}

//! Returns a program ID based on a name.
/*!
  \param name is the name of a previously added shader program
*/
GLuint ShaderManager::getShader(std::string name)
{
  GLuint program_ID = shader_program_IDs[name];
  if (!program_ID)
  {
    std::cout <<
    "ERROR : This name, " <<
    name <<
    " is not a valid shader program name!" <<
    std::endl;
  }
  return program_ID;
}
