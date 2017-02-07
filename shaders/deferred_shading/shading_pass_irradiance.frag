#version 410 core

// Out data
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D irradiance_buffer; // Irradiance

uniform ivec2 window_size;

void main()
{
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
 
  // Material properties
  vec3 irradiance = texture(irradiance_buffer,  sample_point_texture_space).rgb;
  color = vec4(irradiance, 1.0f);
}