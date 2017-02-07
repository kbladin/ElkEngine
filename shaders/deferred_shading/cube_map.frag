#version 410 core

// In data
in vec3 vertex_position_worldspace;
in vec3 vertex_position_viewspace;

// Out data
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D albedo_buffer; // Albedo
uniform ivec2 window_size;

uniform samplerCube cube_map;

void main()
{
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
  float alpha = texture(albedo_buffer, sample_point_texture_space).a;
  vec3 texture_sample = texture(cube_map, vertex_position_worldspace).rgb;

  color = vec4(texture_sample * (1 - alpha), 1.0);
}