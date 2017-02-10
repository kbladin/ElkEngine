#version 410 core

// Out data
layout(location = 0) out vec4 final_irradiance;

// Uniforms
uniform sampler2D albedo_buffer; // Albedo
uniform sampler2D position_buffer; // Position
uniform sampler2D normal_buffer; // Normal
uniform sampler2D material_buffer; // Roughness, Dielectric Fresnel term, metalness
uniform sampler2D irradiance_buffer; // Irradiance

uniform mat4 P_frag;

uniform samplerCube cube_map;
uniform mat3 V_inv;

float castReflectionRay(vec3 origin, vec3 direction, out vec3 radiance, float roughness)
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
    vec3 position = textureLod(position_buffer, position_texture_space, 0).xyz;
    float alpha = textureLod(albedo_buffer, position_texture_space, 7).a;

    if (position_texture_space.x < 0 || position_texture_space.x > 1 ||
        position_texture_space.y < 0 || position_texture_space.y > 1)
    {
      return 0.0f;
    }
    if (position.z > position_view_space.z &&
        (position.z - position_view_space_prev.z) < 0.2 &&
        alpha >= 0.5f)
    {
      float alpha_remapped = (alpha - 0.5f) * 2.0f;
      radiance += textureLod(irradiance_buffer, position_texture_space, 4).rgb;
      return alpha_remapped;
    }
    t += step;
  }
  return 0.0f;
}

vec3 environment(vec3 dir_view_space, float roughness)
{
  //vec3 color = texture(cube_map, dir).rgb;
  vec3 dir_world_space = V_inv * dir_view_space;
  vec3 color = textureLod(cube_map, dir_world_space, pow(roughness, 0.3) * 15).rgb;
  return color;
}

void main()
{
  vec3 specular_radiance_env;
  ivec2 raster_coord = ivec2(gl_FragCoord.xy);
 
  // Material properties
  vec3 irradiance = texelFetch(irradiance_buffer, raster_coord, 0).rgb;
  vec3 position =   texelFetch(position_buffer,   raster_coord, 0).xyz;
  vec4 albedo =     texelFetch(albedo_buffer,     raster_coord, 0);
  
  if (albedo.a > 0.5)
  {
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

    vec3 radiance_reflection = vec3(0);
    float hit = 0.0f;//castReflectionRay(position + n * 0.01f, r, radiance_reflection, roughness);


    // Different Frenel depending on if the material is metal or dielectric
    vec3  R_metal = (albedo.rgb + (vec3(1.0f) - albedo.rgb) * vec3(R));
    vec3  R_specular = vec3(R * (1.0f - metalness)) + R_metal * metalness;

    // Filter radiance through colors and material  
    specular_radiance_env = R_specular * (environment(r, roughness) * (1 - hit) + hit * radiance_reflection) * irradiance_specular_environment;
  }
  // Add to final radiance
  final_irradiance = vec4(max(specular_radiance_env + irradiance, 0.0f), 1.0f);
}