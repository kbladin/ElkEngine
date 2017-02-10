#version 410 core

// In data
in vec3 position_viewspace_vert;

// Out data
out vec4 color;

void main()
{
  color = vec4(1,1,1, 1.0);

  // Write to linear depth buffer
  float max_dist = 1000.0f;
  float depth = (-position_viewspace_vert.z / max_dist);
  gl_FragDepth = depth;
 }