#version 410 core

struct DirectionalLightSource
{
  vec3  direction;
  vec3  color;
  float radiance; // W * Sr^−1 * m^−2 [M * T^-3]
};

// Out data
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D tex0; // Albedo
uniform sampler2D tex1; // Position
uniform sampler2D tex2; // Normal
uniform sampler2D tex3; // Roughness

uniform DirectionalLightSource light_source;

uniform ivec2 window_size;

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

vec3 environment(vec3 dir)
{
  return vec3(0.1, 0.1, 0.1);
}

void main()
{
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
  
  // Material properties
  vec3 albedo =     texture(tex0, sample_point_texture_space).rgb;
  vec3 position =   texture(tex1, sample_point_texture_space).xyz;
  vec3 normal =     texture(tex2, sample_point_texture_space).xyz;
  float roughness = texture(tex3, sample_point_texture_space).x;
  float index_of_refraction = 2;
  
  // Useful vectors
  vec3 n = normalize(normal);
  vec3 l = normalize(light_source.direction);
  vec3 v = normalize(position - vec3(0.0f));
  vec3 r = reflect(v, n);
  
  // Form factors
  float cos_theta = max(dot(n, -l), 0.0f);
  float cos_beta =  max(dot(r, -l), 0.0f);
  float cos_alpha = max(dot(-v, n), 0.0f);

  // Fresnel term
  float R = schlick(1, index_of_refraction, cos_alpha);

  // BRDFs
  float BRDF_diffuse = 1.0;
  float BRDF_specular_times_cos_theta = gaussian(1 - cos_beta, roughness, 0.0f);

  // Irradiance measured in Watts per square meter
  // [M * L^2 * T^-3] * [Sr^-1] * [L^-2] = [M * Sr^-1 * T^-3]
  // Rendering equation over whole hemisphere
  float irradiance_diffuse =  light_source.radiance * BRDF_diffuse      * cos_theta * 2 * PI;
  float irradiance_specular = light_source.radiance * BRDF_specular_times_cos_theta * 2 * PI;

  // Filter radiance through colors and material
  vec3 diffuse_radiance = albedo * (1.0f - R) * light_source.color * irradiance_diffuse;
  vec3 specular_radiance =                 R  * light_source.color * irradiance_specular;

  // Add to final radiance
  color = vec4(diffuse_radiance + specular_radiance, 1.0f);
}