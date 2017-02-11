#version 410 core

struct PointLightSource
{
  vec3  position;     // Position in view space
  vec3  color;        // Works as a filter
  float radiant_flux; // Given in Watt [M * L^2 * T^-3]
};

// Out data
layout(location = 0) out vec4 radiance;

// Uniforms
uniform sampler2D albedo_buffer;    // Albedo
uniform sampler2D position_buffer;  // Position
uniform sampler2D normal_buffer;    // Normal
uniform sampler2D material_buffer;  // Roughness, Dielectric Fresnel term, metalness

uniform PointLightSource light_source;

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
    vec3 position = textureLod(position_buffer, position_texture_space, 0).xyz;
    float alpha = textureLod(albedo_buffer, position_texture_space, 0).a;

    if (position_texture_space.x < 0 || position_texture_space.x > 1 ||
        position_texture_space.y < 0 || position_texture_space.y > 1)
    {
      return 0.0f;
    }
    if (position.z > position_view_space.z &&
        (position.z - position_view_space_prev.z) < 0.2 &&
        alpha != 0.0f)
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
  
  ivec2 raster_coord = ivec2(gl_FragCoord.xy);

  // Material properties
  vec4 albedo =     texelFetch(albedo_buffer,   raster_coord, 0);
  if (albedo.a != 0.0)
  {
    vec3 position =   texelFetch(position_buffer, raster_coord, 0).xyz;
    vec3 normal =     texelFetch(normal_buffer,   raster_coord, 0).xyz;
    float roughness = texelFetch(material_buffer, raster_coord, 0).x;
    float R =         texelFetch(material_buffer, raster_coord, 0).y;
    float metalness = texelFetch(material_buffer, raster_coord, 0).z;

    // Useful vectors
    vec3 n = normalize(normal);
    vec3 light_to_point = position - light_source.position;
    float inv_dist_square = 1.0f / pow(length(light_to_point), 2.0f);
    vec3 l = normalize(light_to_point);
    vec3 v = normalize(position - vec3(0.0f));
    vec3 r = reflect(v, n);

    // Form factors
    float cos_theta = max(dot(n, -l), 0.0f);
    float cos_beta =  max(dot(r, -l), 0.0f);
    float cos_alpha = max(dot(-v, n), 0.0f);

    // BRDFs
    float BRDF_diffuse = 1.0;
    // Roughness = 1 should correspond to a cone angle of 90 degrees
    // (PI / 2 radians). Input to gaussian 1, 1 should correspond to 90 / 2 degrees
    // = PI / 4 radians (half cone). x = PI/4 -> 1 : x = PI/4 / (PI/4)
    // The area under BRDF_specular_times_cos_theta should be the same as the
    // area under BRDF_diffuse which is PI from -PI/2 to PI/2. The area under
    // the gaussian is 1 so we need to divide by (PI / 4.0f) and multiply with PI.
    // In other words multiply woth 4.
    // (Some erros will occur for higher roughness since the gaussian bleeds
    // outside of the defined region, negligable for low roughness).
    float BRDF_specular_times_cos_theta = gaussian(acos(cos_beta) / (PI / 4.0f), roughness, 0.0f) * 4.0f;

    // Irradiance measured in Watts per square meter
    // [M * L^2 * T^-3] * [Sr^-1] * [L^-2] = [M * Sr^-1 * T^-3]
    // Rendering equation over whole hemisphere
    float light_source_radiance = light_source.radiant_flux * inv_dist_square;
    float irradiance_diffuse =  light_source_radiance * BRDF_diffuse      * cos_theta * 2 * PI;
    float irradiance_specular = light_source_radiance * BRDF_specular_times_cos_theta * 2 * PI;

    float hit = 0;// castShadowRay(position + n * 0.01f, -l);

    // Different Frenel depending on if the material is metal or dielectric
    vec3  R_metal = (albedo.rgb + (vec3(1.0f) - albedo.rgb) * vec3(R));
    vec3  R_diffuse = vec3((1.0f - R) * (1.0f - metalness));
    vec3  R_specular = vec3(R * (1.0f - metalness)) + R_metal * metalness;

    // Filter radiance through colors and material
    vec3 diffuse_radiance = albedo.rgb * R_diffuse  * light_source.color * irradiance_diffuse * (1 - hit);
    vec3 specular_radiance =             R_specular * light_source.color * irradiance_specular * (1 - hit);



    total_radiance = diffuse_radiance + specular_radiance;
  }
  // Add to final radiance
  radiance = vec4(total_radiance, 1.0f);
}