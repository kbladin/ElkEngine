#version 410 core

// Out data
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D color_buffer;
uniform ivec2 window_size;

void main()
{
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
 
  // Material properties
  color = vec4(texture(color_buffer, sample_point_texture_space).rgb, 1.0f);
}