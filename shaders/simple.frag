#version 410 core

// In data
in vec3 vertex_normal_viewspace;

// Out data
out vec4 color;

void main()
{
  color = vec4(vertex_normal_viewspace, 1.0);
}