#version 410 core

// Out data
layout(location = 0) out vec3 highlight;

// Uniforms
uniform sampler2D irradiance_buffer; // Irradiance

uniform ivec2 window_size;

void main()
{
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;

  float level = log2(1 / (1 - sample_point_texture_space.x));
  float scale = pow(2, ceil(level));
  float min_pos = 1 - 1 / pow(2, floor(level));
  vec2 sample_pos = vec2(sample_point_texture_space.x - min_pos, sample_point_texture_space.y / 2) * scale;

  // Material properties
  vec3 irradiance = textureLod(irradiance_buffer, sample_pos, clamp(floor(level) + 1, 0, 7)).rgb;

  // Add to final radiance
  highlight = clamp(irradiance - vec3(1.0f), 0.0f, 0.5f);
}