#version 410 core

// Out data
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D tex0; // Albedo
uniform sampler2D tex1; // Position
uniform sampler2D tex2; // Normal
uniform sampler2D tex3; // Roughness

uniform ivec2 window_size;
uniform samplerCube cube_map;
uniform mat3 V_inv;

#define PI 3.1415
float gaussian(float x, float sigma, float mu)
{
  float a = 1.0f / (sigma * sqrt(2.0f * PI));
  float x_minus_b = x - mu;
  return a * exp(-(x_minus_b * x_minus_b) / (2.0f * sigma * sigma));
}

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

vec3 environment(vec3 dir_view_space, float roughness)
{
  //vec3 color = texture(cube_map, dir).rgb;
  vec3 dir_world_space = V_inv * dir_view_space;
  vec3 color = textureLod(cube_map, dir_world_space, pow(roughness, 0.3) * 15).rgb;
  return color;
}

vec3 gammaCorrection(vec3 v, float gamma)
{
  return vec3(pow(v.x, gamma), pow(v.y, gamma), pow(v.z, gamma));
}

void main()
{
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
  
  // Material properties
  vec4 albedo =     texture(tex0, sample_point_texture_space);
  vec3 position =   texture(tex1, sample_point_texture_space).xyz;
  vec3 normal =     texture(tex2, sample_point_texture_space).xyz;
  float roughness = texture(tex3, sample_point_texture_space).x;
  float index_of_refraction = texture(tex3, sample_point_texture_space).y;

  // Useful vectors
  vec3 n = normalize(normal);
  vec3 v = normalize(position - vec3(0.0f));
  vec3 r = reflect(v, n);

  // Form factors
  float cos_alpha = max(dot(-v, n), 0.0f);

  // Fresnel term
  //float R = schlick(1, index_of_refraction, cos_alpha);
  float R = roughSchlick2(1, index_of_refraction, cos_alpha, roughness);

  // BRDFs
  float BRDF_specular_times_cos_theta_at_reflection = 1.0f;

  // Irradiance measured in Watts per square meter
  // [M * L^2 * T^-3] * [Sr^-1] * [L^-2] = [M * Sr^-1 * T^-3]
  // Rendering equation over whole hemisphere
  float irradiance_specular_environment = 1.0f * BRDF_specular_times_cos_theta_at_reflection;

  // Filter radiance through colors and material
  vec3 specular_radiance_env =              R  * environment(r, roughness) * irradiance_specular_environment;
  vec3 diffuse_radiance_env = albedo.rgb * (1 - R) * environment(n, 0.5)        * 1.0f;

  // Add to final radiance
  color = vec4((specular_radiance_env + diffuse_radiance_env) * albedo.a, 1.0f);
}