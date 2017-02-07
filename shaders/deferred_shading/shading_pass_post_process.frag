#version 410 core

// Out data
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D irradiance_buffer; // Irradiance
uniform sampler2D bloom_buffer;

uniform ivec2 window_size;
uniform ivec2 bloom_buffer_base_size;

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

void main()
{
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
 
  // Material properties
  vec3 irradiance = texture(irradiance_buffer,  sample_point_texture_space).rgb;
	vec3 bloom;

	bloom += sampleBloomMipmap(sample_point_texture_space, 0);
	bloom += sampleBloomMipmap(sample_point_texture_space, 1);
	bloom += sampleBloomMipmap(sample_point_texture_space, 2);
	bloom += sampleBloomMipmap(sample_point_texture_space, 3);
	bloom += sampleBloomMipmap(sample_point_texture_space, 4);
	bloom += sampleBloomMipmap(sample_point_texture_space, 5);
	bloom += sampleBloomMipmap(sample_point_texture_space, 6);
	bloom += sampleBloomMipmap(sample_point_texture_space, 7);

	bloom /= 8.0f;
	
  // Add to final radiance
  color = vec4(irradiance + bloom, 1.0f);
}