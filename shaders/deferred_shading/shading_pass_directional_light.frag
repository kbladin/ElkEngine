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
uniform mat4 P_frag;

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

vec3 environment(vec3 dir)
{
  return vec3(0.1, 0.1, 0.1);
}

float castShadowRay(vec3 origin, vec3 direction)
{
  float hit = 0.0f;
  float step = 0.1;
  float t = 0.0f;
  for (int i = 0; i < 10; i++)
  {
    vec3 position_view_space = origin + t * direction;
    vec4 position_screen_space = P_frag * vec4(position_view_space, 1.0f);
    position_screen_space /= position_screen_space.w;
    vec2 position_texture_space = position_screen_space.xy * 0.5f + vec2(0.5f);
    vec3 position = texture(tex1, position_texture_space).xyz;
    float alpha = texture(tex0, position_texture_space).a;

    hit += (position.z > position_view_space.z ? 1.0f : 0.0f) * alpha;
      
    t += step;
  }
  return 1.0f - clamp(hit, 0.0f, 1.0f);
}

void main()
{
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
  
  // Material properties
  vec4 albedo =     texture(tex0, sample_point_texture_space);
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
  float R = roughSchlick2(1, index_of_refraction, cos_alpha, roughness);

  // BRDFs
  float BRDF_diffuse = 1.0;
  float BRDF_specular_times_cos_theta = gaussian(1 - cos_beta, roughness, 0.0f);

  // Irradiance measured in Watts per square meter
  // [M * L^2 * T^-3] * [Sr^-1] * [L^-2] = [M * Sr^-1 * T^-3]
  // Rendering equation over whole hemisphere
  float irradiance_diffuse =  light_source.radiance * BRDF_diffuse      * cos_theta * 2 * PI;
  float irradiance_specular = light_source.radiance * BRDF_specular_times_cos_theta * 2 * PI;

  // Filter radiance through colors and material
  vec3 diffuse_radiance = albedo.rgb * (1.0f - R) * light_source.color * irradiance_diffuse;
  vec3 specular_radiance =                 R  * light_source.color * irradiance_specular;

  // Add to final radiance
  color = vec4((diffuse_radiance + specular_radiance) * albedo.a * castShadowRay(position + n * 0.01f, -l), 1.0f);
}