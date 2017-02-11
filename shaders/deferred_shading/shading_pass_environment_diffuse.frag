#version 410 core

// Out data
layout(location = 0) out vec4 radiance;

// Uniforms
uniform sampler2D albedo_buffer; // Albedo
uniform sampler2D position_buffer; // Position
uniform sampler2D normal_buffer; // Normal
uniform sampler2D material_buffer; // Roughness, Dielectric Fresnel term, metalness

uniform samplerCube cube_map;
uniform int cube_map_size;
uniform mat3 V_inv;

vec3 environment(vec3 dir_view_space, float roughness)
{
  float level = clamp(log2(roughness * cube_map_size), 0, 10);
  vec3 dir_world_space = V_inv * dir_view_space;
  vec3 color = textureLod(cube_map, dir_world_space, level).rgb;
  return color;
}

void main()
{
  vec3 diffuse_radiance_env;
  ivec2 raster_coord = ivec2(gl_FragCoord.xy);
  
  // Material properties
  vec4 albedo =     texelFetch(albedo_buffer,   raster_coord, 0);
  if (albedo.a > 0.5)
  {
    vec3 position =   texelFetch(position_buffer, raster_coord, 0).xyz;
    vec3 normal =     texelFetch(normal_buffer,   raster_coord, 0).xyz;
    float roughness = texelFetch(material_buffer, raster_coord, 0).x;
    float R =         texelFetch(material_buffer, raster_coord, 0).y;
    float metalness = texelFetch(material_buffer, raster_coord, 0).z;

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
    diffuse_radiance_env = albedo.rgb * R_diffuse * environment(n, 1) * 1.0f;
  }
  // Add to final radiance
  radiance = vec4(diffuse_radiance_env, 1.0f);
}