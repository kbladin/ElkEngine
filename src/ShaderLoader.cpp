#include "SGE/ShaderLoader.h"
#include <fstream>

#include <GL/glew.h>

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

//! Load and compile a shader program.
/*!
 Function referenced from www.opengl-tutorials.org.
*/
GLuint ShaderLoader::loadShaders(
  const char * vertex_file_path,
  const char * tesselation_control_file_path,
  const char * tesselation_eval_file_path,
  const char * geometry_file_path,
  const char * fragment_file_path)
{
  GLuint vertex_shader_ID;
  GLuint fragment_shader_ID;
  GLuint tesselation_control_shader_ID;
  GLuint tesselation_evaluation_shader_ID;
  GLuint geometry_shader_ID;
  
  // Create the shaders (vertex and fragment shader must be in the pipeline)
  if (vertex_file_path)
    vertex_shader_ID = glCreateShader(GL_VERTEX_SHADER);
  else
  {
    printf("ERROR : A vertex shader was not provided");
  }
  if (tesselation_control_file_path)
    tesselation_control_shader_ID = glCreateShader(GL_TESS_CONTROL_SHADER);
  if (tesselation_eval_file_path)
    tesselation_evaluation_shader_ID = glCreateShader(GL_TESS_EVALUATION_SHADER);
  if (geometry_file_path)
    geometry_shader_ID = glCreateShader(GL_GEOMETRY_SHADER);
  if (fragment_file_path)
    fragment_shader_ID = glCreateShader(GL_FRAGMENT_SHADER);
  else
  {
    printf("ERROR : A fragment shader was not provided");
  }

  // Read the Vertex Shader code from the file
  std::string vertex_shader_code;
  std::ifstream vertex_shader_stream(vertex_file_path, std::ios::in);
  if(vertex_shader_stream.is_open())
  {
    std::string Line = "";
    while(getline(vertex_shader_stream, Line))
      vertex_shader_code += "\n" + Line;
    vertex_shader_stream.close();
  }
  else
  {
    printf("ERROR : %s could not be opened.\n", vertex_file_path);
  }
  
  std::string tesselation_control_shader_code;
  if (tesselation_control_file_path){
    // Read the Tesselation Control Shader code from the file
    std::ifstream tesselation_control_shader_stream(tesselation_control_file_path, std::ios::in);
    if(tesselation_control_shader_stream.is_open()){
      std::string Line = "";
      while(getline(tesselation_control_shader_stream, Line))
        tesselation_control_shader_code += "\n" + Line;
      tesselation_control_shader_stream.close();
    }
    else
    {
      printf("ERROR : %s could not be opened.\n", tesselation_control_file_path);
    }
  }

  std::string tesselation_evaluation_shader_code;
  if (tesselation_eval_file_path){
    // Read the Tesselation Evaluation Shader code from the file
    std::ifstream tesselation_eval_stream(tesselation_eval_file_path, std::ios::in);
    if(tesselation_eval_stream.is_open()){
      std::string Line = "";
      while(getline(tesselation_eval_stream, Line))
        tesselation_evaluation_shader_code += "\n" + Line;
      tesselation_eval_stream.close();
    }
    else
    {
      printf("ERROR : %s could not be opened.\n", tesselation_eval_file_path);
    }
  }

  std::string geometry_shader_code;
  if (geometry_file_path){
    // Read the Geometry Shader code from the file
    std::ifstream geometry_shader_stream(geometry_file_path, std::ios::in);
    if(geometry_shader_stream.is_open()){
      std::string Line = "";
      while(getline(geometry_shader_stream, Line))
        geometry_shader_code += "\n" + Line;
      geometry_shader_stream.close();
    }
    else
    {
      printf("ERROR : %s could not be opened.\n", geometry_file_path);
    }
  }

  // Read the Fragment Shader code from the file
  std::string fragment_shader_code;
  std::ifstream fragment_shader_stream(fragment_file_path, std::ios::in);
  if(fragment_shader_stream.is_open()){
    std::string Line = "";
    while(getline(fragment_shader_stream, Line))
      fragment_shader_code += "\n" + Line;
    fragment_shader_stream.close();
  }
  else
  {
    printf("ERROR : %s could not be opened.\n", fragment_file_path);
  }
  
  GLint result = GL_FALSE;
  int info_log_length;
  
  // Compile Vertex Shader
  printf("Compiling shader : %s\n", vertex_file_path);
  char const * VertexSourcePointer = vertex_shader_code.c_str();
  glShaderSource(vertex_shader_ID, 1, &VertexSourcePointer , NULL);
  glCompileShader(vertex_shader_ID);
  
  // Check Vertex Shader
  glGetShaderiv(vertex_shader_ID, GL_COMPILE_STATUS, &result);
  glGetShaderiv(vertex_shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);
  std::vector<char> vertex_shader_error_message(info_log_length);
  glGetShaderInfoLog(vertex_shader_ID, info_log_length, NULL, &vertex_shader_error_message[0]);
  //if (strlen(&vertex_shader_error_message[0]) > 0)
    fprintf(stdout, "%s\n", &vertex_shader_error_message[0]);

  if (tesselation_control_file_path){
    // Compile Tesselation Control Shader
    printf("Compiling shader : %s\n", tesselation_control_file_path);
    char const * TesselationControlSourcePointer = tesselation_control_shader_code.c_str();
    glShaderSource(tesselation_control_shader_ID, 1, &TesselationControlSourcePointer , NULL);
    glCompileShader(tesselation_control_shader_ID);
    
    // Check Tesselation Control Shader
    glGetShaderiv(tesselation_control_shader_ID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(tesselation_control_shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    std::vector<char> tesselation_control_shader_error_message(info_log_length);
    glGetShaderInfoLog(tesselation_control_shader_ID, info_log_length, NULL, &tesselation_control_shader_error_message[0]);
    //if (strlen(&tesselation_control_shader_error_message[0]) > 0)
      fprintf(stdout, "%s\n", &tesselation_control_shader_error_message[0]);
  }

  if (tesselation_eval_file_path){
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", tesselation_eval_file_path);
    char const * TesselationEvaluationSourcePointer = tesselation_evaluation_shader_code.c_str();
    glShaderSource(tesselation_evaluation_shader_ID, 1, &TesselationEvaluationSourcePointer , NULL);
    glCompileShader(tesselation_evaluation_shader_ID);
    
    // Check TesselationEvaluation Shader
    glGetShaderiv(tesselation_evaluation_shader_ID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(tesselation_evaluation_shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    std::vector<char> tesselation_evaluation_shader_error_message(info_log_length);
    glGetShaderInfoLog(tesselation_evaluation_shader_ID, info_log_length, NULL, &tesselation_evaluation_shader_error_message[0]);
    //if (strlen(&tesselation_evaluation_shader_error_message[0]) > 0)
      fprintf(stdout, "%s\n", &tesselation_evaluation_shader_error_message[0]);
  }

  if (geometry_file_path){
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", geometry_file_path);
    char const * GeometrySourcePointer = geometry_shader_code.c_str();
    glShaderSource(geometry_shader_ID, 1, &GeometrySourcePointer , NULL);
    glCompileShader(geometry_shader_ID);
    
    // Check Geometry Shader
    glGetShaderiv(geometry_shader_ID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(geometry_shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    std::vector<char> geometry_shader_error_message(info_log_length);
    glGetShaderInfoLog(geometry_shader_ID, info_log_length, NULL, &geometry_shader_error_message[0]);
    //if (strlen(&geometry_shader_error_message[0]) > 0)
      fprintf(stdout, "%s\n", &geometry_shader_error_message[0]);
  }

  // Compile Fragment Shader
  printf("Compiling shader : %s\n", fragment_file_path);
  char const * FragmentSourcePointer = fragment_shader_code.c_str();
  glShaderSource(fragment_shader_ID, 1, &FragmentSourcePointer , NULL);
  glCompileShader(fragment_shader_ID);
  
  // Check Fragment Shader
  glGetShaderiv(fragment_shader_ID, GL_COMPILE_STATUS, &result);
  glGetShaderiv(fragment_shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);
  std::vector<char> fragment_shader_error_message(info_log_length);
  glGetShaderInfoLog(fragment_shader_ID, info_log_length, NULL, &fragment_shader_error_message[0]);
  //if (strlen(&fragment_shader_error_message[0]) > 0)
    fprintf(stdout, "%s\n", &fragment_shader_error_message[0]);
  
  // Link the program
  fprintf(stdout, "Linking program\n");
  GLuint program_ID = glCreateProgram();
  glAttachShader(program_ID, vertex_shader_ID);
  if (tesselation_control_file_path)
    glAttachShader(program_ID, tesselation_control_shader_ID);
  if (tesselation_eval_file_path)
    glAttachShader(program_ID, tesselation_evaluation_shader_ID);
  if (geometry_file_path)
    glAttachShader(program_ID, geometry_shader_ID);
  glAttachShader(program_ID, fragment_shader_ID);

  glLinkProgram(program_ID);
  
  // Check the program
  glGetProgramiv(program_ID, GL_LINK_STATUS, &result);
  glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &info_log_length);
  std::vector<char> ProgramErrorMessage( std::max(info_log_length, int(1)) );
  glGetProgramInfoLog(program_ID, info_log_length, NULL, &ProgramErrorMessage[0]);
  fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
  
  glDeleteShader(vertex_shader_ID);
  if (tesselation_control_file_path)
    glDeleteShader(tesselation_control_shader_ID);
  if (tesselation_eval_file_path)
    glDeleteShader(tesselation_evaluation_shader_ID);
  if (geometry_file_path)
    glDeleteShader(geometry_shader_ID);
  glDeleteShader(fragment_shader_ID);

  return program_ID;
}