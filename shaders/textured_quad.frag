#version 410 core

// In data
in vec2 fs_texture_coordinate;

// Out data
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D final_irradiance_buffer;

void main()
{
  vec3 albedo = texture(final_irradiance_buffer, fs_texture_coordinate).rgb;
  color = vec4(albedo, 1.0f);
}