#version 410 core

// Out data
layout(location = 0) out vec3 highlight;

// Uniforms
uniform sampler2D irradiance_buffer; // Irradiance

uniform ivec2 window_size;

void main()
{
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
  vec3 irradiance = textureLod(irradiance_buffer, sample_point_texture_space, 0).rgb;
  highlight = max(irradiance - vec3(1.0f), 0.0f);
}