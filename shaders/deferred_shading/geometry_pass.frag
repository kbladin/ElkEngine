#version 410 core

// In data
in vec3 vertex_normal_viewspace;
in vec4 vertex_position_viewspace;
in vec2 fs_texture_coordinate;

// Out data
layout(location = 0) out vec4 color;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;

// Uniforms
uniform sampler2D tex;

void main()
{
  color = vec4(1,1,1, 1.0f);
  position = vertex_position_viewspace.xyz;
  normal = vertex_normal_viewspace;
}