#version 410 core

// In data
in vec3 vertex_normal_viewspace;
in vec4 vertex_position_viewspace;
in vec2 fs_texture_coordinate;

// Out data
layout(location = 0) out vec4 albedo;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec3 material; // Roughness, Fresnel Term, metalness

// Uniforms
uniform sampler2D albedo_texture;
uniform sampler2D roughness_texture;
uniform sampler2D IOR_texture;
uniform sampler2D metalness_texture;
uniform sampler2D normal_texture;

uniform float roughness;
uniform float IOR;

float schlick(float n1, float n2, float cos_theta)
{
  float R0 = pow((n1 - n2) / (n1 + n2), 2);
  float R = R0 + (1 - R0) * pow((1 - cos_theta), 5);
  return R;
}

float roughSchlick2(float n1, float n2, float cos_theta, float roughness)
{
  float R0 = pow((n1 - n2) / (n1 + n2), 2);
  float area_under_curve = 1.0 / 6.0 * (5.0 * R0 + 1.0);
  float new_area_under_curve = 1.0 / (6.0 * roughness + 6.0) * (5.0 * R0 + 1.0);

  return schlick(n1, n2, cos_theta) / (1 + roughness) + (area_under_curve - new_area_under_curve);
}

void main()
{
  position = vertex_position_viewspace.xyz;

  vec3 sampled_normal = texture(normal_texture, fs_texture_coordinate).xyz;
  normal = length(sampled_normal) == 0.0f ? normalize(vertex_normal_viewspace) : sampled_normal;

  albedo =            texture(albedo_texture, fs_texture_coordinate);
  float _roughness =  texture(roughness_texture, fs_texture_coordinate).r;
  float _IOR =        texture(IOR_texture, fs_texture_coordinate).r;
  float metalness =   texture(metalness_texture, fs_texture_coordinate).r;

  // Calculate dielctric Fresnel term
  vec3 v = normalize(position);
  float cos_theta = max(dot(-v, normal),  0.0f);
  float fresnel_term = roughSchlick2(1, IOR, cos_theta, _roughness * roughness + 0.001);

  material = vec3(5.0f * (1.0f / (1.0f - pow(_roughness, 0.5) * 0.5f + 0.001f) - 1.0f) * pow(_roughness, 1.5) + 0.001f, fresnel_term, metalness);
}