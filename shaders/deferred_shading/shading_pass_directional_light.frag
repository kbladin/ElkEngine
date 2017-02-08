#version 410 core

struct DirectionalLightSource
{
  vec3  direction;  // Direction in view space
  vec3  color;      // Works as a filter
  float radiance;   // W * Sr^−1 * m^−2 [M * T^-3]
};

// Out data
layout(location = 0) out vec4 radiance;

// Uniforms
uniform sampler2D albedo_buffer;    // Albedo
uniform sampler2D position_buffer;  // Position
uniform sampler2D normal_buffer;    // Normal
uniform sampler2D material_buffer;  // Roughness, Dielectric Fresnel term, metalness

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

float castShadowRay(vec3 origin, vec3 direction)
{
  float step = 0.1;
  float t = 0.0f;
  vec3 position_view_space = vec3(0.0f,0.0f,0.0f);

  for (int i = 0; i < 50; i++)
  {
    vec3 position_view_space_prev = position_view_space;
    position_view_space = origin + t * direction;
    vec4 position_clip_space = P_frag * vec4(position_view_space, 1.0f);
    vec3 position_screen_space = position_clip_space.xyz / position_clip_space.w;
    vec2 position_texture_space = position_screen_space.xy * 0.5f + vec2(0.5f);
    vec3 position = texture(position_buffer, position_texture_space).xyz;
    float alpha = texture(albedo_buffer, position_texture_space).a;

    if (position_texture_space.x < 0 || position_texture_space.x > 1 ||
        position_texture_space.y < 0 || position_texture_space.y > 1)
    {
      return 0.0f;
    }
    if (position.z > position_view_space.z && (position.z - position_view_space_prev.z) < 0.2 && alpha != 0.0f)
    {
      return 1.0f;
    }
    t += step;
  }
  return 0.0f;
}

void main()
{
  vec3 total_radiance;
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
  
  // Material properties
  vec4 albedo = texture(albedo_buffer, sample_point_texture_space);
  if (albedo.a != 0.0)
  {
    vec3 position =   texture(position_buffer, sample_point_texture_space).xyz;
    vec3 normal =     texture(normal_buffer,   sample_point_texture_space).xyz;
    float roughness = texture(material_buffer, sample_point_texture_space).x;
    float R =         texture(material_buffer, sample_point_texture_space).y;
    float metalness = texture(material_buffer, sample_point_texture_space).z;

    // Useful vectors
    vec3 n = normalize(normal);

    vec3 l = normalize(light_source.direction);
    vec3 v = normalize(position - vec3(0.0f));
    vec3 r = reflect(v, n);
    
    // Form factors
    float cos_theta = max(dot(n, -l), 0.0f);
    float cos_beta =  max(dot(r, -l), 0.0f);
    float cos_alpha = max(dot(-v, n), 0.0f);

    // BRDFs
    float BRDF_diffuse = 1.0;
    float BRDF_specular_times_cos_theta = gaussian(1 - cos_beta, roughness, 0.0f);

    // Irradiance measured in Watts per square meter
    // [M * L^2 * T^-3] * [Sr^-1] * [L^-2] = [M * Sr^-1 * T^-3]
    // Rendering equation over whole hemisphere

    float irradiance_diffuse =  light_source.radiance * BRDF_diffuse      * cos_theta * 2 * PI;
    float irradiance_specular = light_source.radiance * BRDF_specular_times_cos_theta * 2 * PI;

    float hit = castShadowRay(position + n * 0.01f, -l);

    // Different Frenel depending on if the material is metal or dielectric
    vec3  R_metal = (albedo.rgb + (vec3(1.0f) - albedo.rgb) * vec3(R));
    vec3  R_diffuse = vec3((1.0f - R) * (1.0f - metalness));
    vec3  R_specular = vec3(R * (1.0f - metalness)) + R_metal * metalness;

    // Filter radiance through colors and material
    vec3 diffuse_radiance = albedo.rgb * R_diffuse  * light_source.color * irradiance_diffuse * (1 - hit);
    vec3 specular_radiance =             R_specular * light_source.color * irradiance_specular * (1 - hit);

    // Hack to avoid hard edge
    specular_radiance *= pow(cos_theta, 0.5);
    total_radiance = diffuse_radiance + specular_radiance;
  }
  // Add to final radiance
  radiance = vec4(total_radiance, 1.0f);
}