#version 410 core

// In data
in vec3 vertex_position_worldspace;

// Out data
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D tex0; // Albedo
uniform ivec2 window_size;

uniform samplerCube cube_map;

void main()
{
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
  float alpha = texture(tex0, sample_point_texture_space).a;
  vec3 texture_sample = texture(cube_map, normalize(vertex_position_worldspace)).rgb;

  color = vec4(texture_sample * (1 - alpha), 1.0);
}