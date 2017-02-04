#version 410 core

// In data
layout(location = 0) in vec3 position;

// Out data
out vec3 vertex_position_worldspace;
out vec3 vertex_position_viewspace;

// Uniform data
// Transform matrices
uniform mat4 V = mat4(1.0f);
uniform mat4 P = mat4(1.0f);

void main()
{
  // Only do rotation
  vertex_position_worldspace = position;
  vertex_position_viewspace = mat3(V) * position;  
  gl_Position = P * vec4(vertex_position_viewspace, 1.0f);
}