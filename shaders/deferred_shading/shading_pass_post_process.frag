#version 410 core

// Out data
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D final_irradiance_buffer; // Irradiance
uniform sampler2D position_buffer; // Position
uniform sampler2D albedo_buffer; // Position
uniform sampler2D bloom_buffer;

uniform ivec2 window_size;
uniform ivec2 bloom_buffer_base_size;

uniform float aperture;
uniform float focus;

vec3 sampleBloomMipmap(vec2 texture_coordinate, int level)
{
	float scale = 1.0f / pow(2, level + 1);
	float min_pos = 1.0f - scale * 2.0f;
	vec2 sample_pos = vec2(min_pos + texture_coordinate.x * scale, texture_coordinate.y * scale);

	vec3 bloom;

	bloom += texture(bloom_buffer, sample_pos).rgb;
	bloom += texture(bloom_buffer, sample_pos + vec2(0.5,0.5) / (bloom_buffer_base_size)).rgb;
	bloom += texture(bloom_buffer, sample_pos - vec2(0.5,0.5) / (bloom_buffer_base_size)).rgb;
	bloom += texture(bloom_buffer, sample_pos + vec2(-0.5,0.5) / (bloom_buffer_base_size)).rgb;
	bloom += texture(bloom_buffer, sample_pos - vec2(-0.5,0.5) / (bloom_buffer_base_size)).rgb;
	//bloom += texture(bloom_buffer, sample_pos + vec2(0,0.8) / (bloom_buffer_base_size)).rgb * 2.0f / 16.0f;
	//bloom += texture(bloom_buffer, sample_pos - vec2(0,0.8) / (bloom_buffer_base_size)).rgb * 2.0f / 16.0f;

	//bloom += texture(bloom_buffer, sample_pos + vec2(0.6,0.6) / (bloom_buffer_base_size)).rgb * 1.0f / 16.0f;
	//bloom += texture(bloom_buffer, sample_pos - vec2(0.6,0.6) / (bloom_buffer_base_size)).rgb * 1.0f / 16.0f;
	//bloom += texture(bloom_buffer, sample_pos + vec2(-0.6,0.6) / (bloom_buffer_base_size)).rgb * 1.0f / 16.0f;
	//bloom += texture(bloom_buffer, sample_pos + vec2(0.6,-0.6) / (bloom_buffer_base_size)).rgb * 1.0f / 16.0f;

	return bloom / 5.0f;
}

#define PI 3.1415

void main()
{
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
  ivec2 raster_coord = ivec2(gl_FragCoord.xy);

  // Material properties
  vec3 position = texelFetch(position_buffer, raster_coord, 0).xyz;
  float alpha = texelFetch(albedo_buffer, raster_coord, 0).a;

  float l = aperture;
  float d = focus;
  float theta = atan(l / d);
  float level_inf = theta / (PI / 2) * 7;

  float level;
  if (alpha == 0)
  	level = level_inf;
  else
  	level = abs((level_inf * d / position.z) + level_inf);

	vec3 irradiance;
  	irradiance += texture(final_irradiance_buffer, sample_point_texture_space, clamp(level, 0, 15)).rgb;
  	irradiance += texture(final_irradiance_buffer, sample_point_texture_space + 1 / vec2(window_size) * level * 0.5 * vec2(1,0), clamp(level, 0, 7)).rgb;
  	irradiance += texture(final_irradiance_buffer, sample_point_texture_space + 1 / vec2(window_size) * level * 0.5 * vec2(0,1), clamp(level, 0, 7)).rgb;
  	irradiance += texture(final_irradiance_buffer, sample_point_texture_space + 1 / vec2(window_size) * level * 0.5 * vec2(-1,0), clamp(level, 0, 7)).rgb;
  	irradiance += texture(final_irradiance_buffer, sample_point_texture_space + 1 / vec2(window_size) * level * 0.5 * vec2(0,-1), clamp(level, 0, 7)).rgb;

  	irradiance += texture(final_irradiance_buffer, sample_point_texture_space + 1 / vec2(window_size) * level * 0.5 * vec2(1,1), clamp(level, 0, 7)).rgb;
  	irradiance += texture(final_irradiance_buffer, sample_point_texture_space + 1 / vec2(window_size) * level * 0.5 * vec2(1,-1), clamp(level, 0, 7)).rgb;
  	irradiance += texture(final_irradiance_buffer, sample_point_texture_space + 1 / vec2(window_size) * level * 0.5 * vec2(-1,1), clamp(level, 0, 7)).rgb;
  	irradiance += texture(final_irradiance_buffer, sample_point_texture_space + 1 / vec2(window_size) * level * 0.5 * vec2(-1,-1), clamp(level, 0, 7)).rgb;
  	irradiance /= 9.0f;

	vec3 bloom;

	bloom += sampleBloomMipmap(sample_point_texture_space, clamp(int(level) + 0, 0, 7));
	bloom += sampleBloomMipmap(sample_point_texture_space, clamp(int(level) + 1, 0, 7));
	bloom += sampleBloomMipmap(sample_point_texture_space, clamp(int(level) + 2, 0, 7));
	bloom += sampleBloomMipmap(sample_point_texture_space, clamp(int(level) + 3, 0, 7));
	bloom += sampleBloomMipmap(sample_point_texture_space, clamp(int(level) + 4, 0, 7));
	bloom += sampleBloomMipmap(sample_point_texture_space, clamp(int(level) + 5, 0, 7));
	bloom += sampleBloomMipmap(sample_point_texture_space, clamp(int(level) + 6, 0, 7));
	bloom += sampleBloomMipmap(sample_point_texture_space, clamp(int(level) + 7, 0, 7));

	bloom /= 8.0f;

  // Add to final radiance
  color = vec4(irradiance + bloom, 1.0f);
}