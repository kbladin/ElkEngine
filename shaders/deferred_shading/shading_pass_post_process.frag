#version 410 core

// Out data
layout(location = 0) out vec4 color;

in vec3 vertex_position_viewspace_unprojected;

// Uniforms
uniform sampler2D final_irradiance_buffer;
uniform sampler2D position_buffer;
uniform sampler2D albedo_buffer;
uniform sampler2D bloom_buffer;

uniform ivec2 window_size;
uniform ivec2 bloom_buffer_base_size;

uniform float aperture;
uniform float focus;
uniform float focal_length;

vec3 sampleBloomMipmap(vec2 texture_coordinate, float level)
{
  vec3 bloom;

  bloom += textureLod(bloom_buffer, texture_coordinate, level).rgb * 2.0 / 10.0f;
  bloom += textureLod(bloom_buffer, texture_coordinate + pow(2,level) * vec2(0,1) / (bloom_buffer_base_size), level).rgb / 10.0f;
  bloom += textureLod(bloom_buffer, texture_coordinate - pow(2,level) * vec2(0,1) / (bloom_buffer_base_size), level).rgb / 10.0f;
  bloom += textureLod(bloom_buffer, texture_coordinate + pow(2,level) * vec2(1,0) / (bloom_buffer_base_size), level).rgb / 10.0f;
  bloom += textureLod(bloom_buffer, texture_coordinate - pow(2,level) * vec2(1,0) / (bloom_buffer_base_size), level).rgb / 10.0f;

  bloom += textureLod(bloom_buffer, texture_coordinate + pow(2,level) * vec2(0.7,0.7) / (bloom_buffer_base_size), level).rgb / 10.0f;
  bloom += textureLod(bloom_buffer, texture_coordinate - pow(2,level) * vec2(0.7,0.7) / (bloom_buffer_base_size), level).rgb / 10.0f;
  bloom += textureLod(bloom_buffer, texture_coordinate + pow(2,level) * vec2(-0.7,0.7) / (bloom_buffer_base_size), level).rgb / 10.0f;
  bloom += textureLod(bloom_buffer, texture_coordinate - pow(2,level) * vec2(-0.7,0.7) / (bloom_buffer_base_size), level).rgb / 10.0f;

  return bloom;
}

#define PI 3.1415

void main()
{
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
  ivec2 raster_coord = ivec2(gl_FragCoord.xy);

  // Material properties
  vec3 position = texelFetch(position_buffer, raster_coord, 0).xyz;
  float alpha = texelFetch(albedo_buffer, raster_coord, 0).a;

  float object_dist = abs(position.z);

  float object_focus;
  if (alpha == 0)
    object_focus = focal_length; // Infinitely far away
  else
    object_focus = 1.0f / (1.0f / focal_length - 1.0f / object_dist);

  float max_level = 7; // Corresponds to completely out of focus
  float level = aperture * 10 * max_level * log2(1.0f + 1.0f / focal_length * abs(object_focus - (focal_length + focus)));

  level = clamp(level, 0, max_level);
  vec3 irradiance;
  irradiance += texture(final_irradiance_buffer, sample_point_texture_space, level).rgb * 2.0f / 10.0f;

  irradiance += textureLod(final_irradiance_buffer, sample_point_texture_space + pow(2,level) * vec2(0,1) / (window_size), level).rgb / 10.0f;
  irradiance += textureLod(final_irradiance_buffer, sample_point_texture_space - pow(2,level) * vec2(0,1) / (window_size), level).rgb / 10.0f;
  irradiance += textureLod(final_irradiance_buffer, sample_point_texture_space + pow(2,level) * vec2(1,0) / (window_size), level).rgb / 10.0f;
  irradiance += textureLod(final_irradiance_buffer, sample_point_texture_space - pow(2,level) * vec2(1,0) / (window_size), level).rgb / 10.0f;

  irradiance += textureLod(final_irradiance_buffer, sample_point_texture_space + pow(2,level) * vec2(0.7,0.7) / (window_size), level).rgb / 10.0f;
  irradiance += textureLod(final_irradiance_buffer, sample_point_texture_space - pow(2,level) * vec2(0.7,0.7) / (window_size), level).rgb / 10.0f;
  irradiance += textureLod(final_irradiance_buffer, sample_point_texture_space + pow(2,level) * vec2(-0.7,0.7) / (window_size), level).rgb / 10.0f;
  irradiance += textureLod(final_irradiance_buffer, sample_point_texture_space - pow(2,level) * vec2(-0.7,0.7) / (window_size), level).rgb / 10.0f;


  vec3 bloom;

  bloom += sampleBloomMipmap(sample_point_texture_space, clamp(0 + level, 0, 7));
  bloom += sampleBloomMipmap(sample_point_texture_space, clamp(1 + level, 0, 7));
  bloom += sampleBloomMipmap(sample_point_texture_space, clamp(2 + level, 0, 7));
  bloom += sampleBloomMipmap(sample_point_texture_space, clamp(3 + level, 0, 7));
  bloom += sampleBloomMipmap(sample_point_texture_space, clamp(4 + level, 0, 7));
  bloom += sampleBloomMipmap(sample_point_texture_space, clamp(5 + level, 0, 7));

  bloom /= 6.0f * 2; // Cheat because the blooming effect was really big (cant handle HDR)

  vec3 total_irradiance = irradiance + bloom;

  // Reinhard tone mapping
  //vec3 mapped = total_irradiance / (total_irradiance + vec3(1.0));
  // Gamma correction 
  //mapped = pow(mapped, vec3(1.0 / 1.1));



  vec3 v = normalize(vertex_position_viewspace_unprojected);
  float vignetting = dot(-v, vec3(0.0f,0.0f,1.0f));

  // Add to final radiance
  color = vec4(vignetting * total_irradiance, 1.0f);
}