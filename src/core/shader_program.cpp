#include "elk/core/shader_program.h"

#include "elk/core/file_utils.h"

#include <array>
#include <vector>

namespace elk { namespace core {

std::stack<GLuint> ShaderProgram::_shader_stack;

ShaderProgram::ShaderProgram(
	std::string name,
	const char* vs_src,
	const char* tcs_src,
	const char* tes_src,
	const char* gs_src,
	const char* fs_src) :
  _name(name)
{
  _id = loadShaderProgram(vs_src, tcs_src, tes_src, gs_src, fs_src);
}

ShaderProgram::~ShaderProgram()
{
  glDeleteProgram(_id);
}

void ShaderProgram::pushUsage()
{
  _shader_stack.push(_id);
  glUseProgram(_shader_stack.top());
}

void ShaderProgram::popUsage()
{
  _shader_stack.pop();
  glUseProgram(_shader_stack.empty() ? 0 : _shader_stack.top());
}

void ShaderProgram::useNone()
{
  _shader_stack = std::stack<GLuint>();
  glUseProgram(0);
}

// https://www.omniref.com/ruby/gems/opengl-bindings/1.3.5/symbols/OpenGL::GL_TESS_CONTROL_SHADER
#ifndef GL_TESS_CONTROL_SHADER
    #define GL_TESS_CONTROL_SHADER 0x8E88
#endif
#ifndef GL_TESS_EVALUATION_SHADER
    #define GL_TESS_EVALUATION_SHADER 0x8E87
#endif
#ifndef GL_GEOMETRY_SHADER
    #define GL_GEOMETRY_SHADER 0x8DD9
#endif
#ifndef GL_PATCHES
    #define GL_PATCHES 0x000E
#endif

GLuint ShaderProgram::loadShaderProgram(
    const char* vs_src,
    const char* tcs_src,
    const char* tes_src,
    const char* gs_src,
    const char* fs_src)
{
  std::array<const char*, 5> paths = {{vs_src,tcs_src,tes_src,gs_src,fs_src}};
  std::array<GLenum, 5> types = {{
    GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER,
    GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER}};
  std::array<std::string, 5> types_names = {{
    "vertex shader", "tesselation control shader", "tesselation evaluation shader",
    "geometry shader", "fragment shader"}};
  std::array<GLuint, 5> ids = {{0,0,0,0,0}};
  std::array<std::string, 5> code;

  GLint result = 0;
  int info_log_length;
  GLuint program_id = glCreateProgram();

  fprintf(stdout, "Creating shader program '%s'\n", _name.c_str());

  for (int i = 0; i < ids.size(); ++i)
  {
    // Try to create shader
    code[i] = paths[i] ? read_file(paths[i])  : "";
    ids[i] = (code[i] != "") ? glCreateShader(types[i]) : 0;
    
    if (ids[i])
    {
      // Try to compile Shader
      char const* code_addr = code[i].c_str();
      glShaderSource(ids[i], 1, &code_addr, NULL);
      glCompileShader(ids[i]);
      
      // Check shader
      glGetShaderiv(ids[i], GL_COMPILE_STATUS, &result);
      glGetShaderiv(ids[i], GL_INFO_LOG_LENGTH, &info_log_length);
      std::vector<char> error_message(info_log_length);
      glGetShaderInfoLog(ids[i], info_log_length, NULL, &error_message[0]);
      if (info_log_length > 0)
      {
        fprintf(stdout,"COMPILATION %s", &error_message[0]);
        fprintf(stdout, "in file %s \n",
          paths[i]);
      }
      glAttachShader(program_id, ids[i]);
      glDeleteShader(ids[i]);
    }
  }

  glLinkProgram(program_id);
  
  // Check for linker errors
  glGetProgramiv(program_id, GL_LINK_STATUS, &result);
  glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
  std::vector<char> error_message( std::max(info_log_length, int(1)) );
  glGetProgramInfoLog(program_id, info_log_length, NULL, &error_message[0]);
  if (info_log_length > 0)
  {
    fprintf(stdout, "LINKING %s\n", &error_message[0]);
  }

  return program_id;
}

} }


