#version 410 core

// Out data
layout(location = 0) out vec4 radiance;

// Uniforms
uniform sampler2D albedo_buffer; // Albedo
uniform sampler2D position_buffer; // Position
uniform sampler2D normal_buffer; // Normal
uniform sampler2D material_buffer; // Roughness, Dielectric Fresnel term, metalness

uniform ivec2 window_size;
uniform mat4 P_frag;

uniform samplerCube cube_map;
uniform mat3 V_inv;

#define PI 3.1415
float gaussian(float x, float sigma, float mu)
{
  float a = 1.0f / (sigma * sqrt(2.0f * PI));
  float x_minus_b = x - mu;
  return a * exp(-(x_minus_b * x_minus_b) / (2.0f * sigma * sigma));
}

vec3 environment(vec3 dir_view_space, float roughness)
{
  vec3 dir_world_space = V_inv * dir_view_space;
  vec3 color = textureLod(cube_map, dir_world_space, pow(roughness, 0.3) * 15).rgb;
  return color;
}

void main()
{
  vec3 diffuse_radiance_env;
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
  
  // Material properties
  vec4 albedo =     texture(albedo_buffer,   sample_point_texture_space);
  if (albedo.a > 0.5)
  {
    vec3 position =   texture(position_buffer, sample_point_texture_space).xyz;
    vec3 normal =     texture(normal_buffer,   sample_point_texture_space).xyz;
    float roughness = texture(material_buffer, sample_point_texture_space).x;
    float R =         texture(material_buffer, sample_point_texture_space).y;
    float metalness = texture(material_buffer, sample_point_texture_space).z;

    // Useful vectors
    vec3 n = normalize(normal);
    vec3 v = normalize(position - vec3(0.0f));
    vec3 r = reflect(v, n);

    // Form factors
    float cos_alpha = max(dot(-v, n), 0.0f);

    // BRDFs
    float BRDF_specular_times_cos_theta_at_reflection = 1.0f;

    // Irradiance measured in Watts per square meter
    // [M * L^2 * T^-3] * [Sr^-1] * [L^-2] = [M * Sr^-1 * T^-3]
    // Rendering equation over whole hemisphere
    float irradiance_specular_environment = 1.0f * BRDF_specular_times_cos_theta_at_reflection;


    // Different Frenel depending on if the material is metal or dielectric
    float R_diffuse = (1.0f - R) * (1.0f - metalness);

    // Filter radiance through colors and material
    diffuse_radiance_env = albedo.rgb * R_diffuse * environment(n, 0.5) * 1.0f;
  }
  // Add to final radiance
  radiance = vec4(diffuse_radiance_env, 1.0f);
}