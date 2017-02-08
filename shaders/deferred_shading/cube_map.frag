#version 410 core

// In data
in vec3 vertex_position_worldspace;
in vec3 vertex_position_viewspace;

// Out data
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D albedo_buffer; // Albedo

uniform samplerCube cube_map;

void main()
{
  ivec2 raster_coord = ivec2(gl_FragCoord.xy);
  float alpha = texelFetch(albedo_buffer, raster_coord, 0).a;
  vec3 texture_sample = texture(cube_map, vertex_position_worldspace).rgb;

  color = vec4(texture_sample * (1 - alpha), 1.0);
}