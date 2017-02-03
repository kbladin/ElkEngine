#version 410 core

// In data
in vec3 vertex_normal_viewspace;
in vec4 vertex_position_viewspace;
in vec2 fs_texture_coordinate;

// Out data
layout(location = 0) out vec4 albedo;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec2 material;

// Uniforms
uniform sampler2D albedo_texture;
uniform sampler2D roughness_texture;
uniform sampler2D IOR_texture;
uniform sampler2D metalness_texture;

uniform float roughness;
uniform float IOR;

void main()
{
  position = vertex_position_viewspace.xyz;
  normal = vertex_normal_viewspace;

  albedo = texture(albedo_texture, fs_texture_coordinate);
  float _roughness = texture(roughness_texture, fs_texture_coordinate).r;
  float _IOR = 2;//texture(IOR_texture, fs_texture_coordinate).r;
  material = vec2(_roughness + 0.001, IOR);
}