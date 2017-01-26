#version 410 core

// In data
in vec3 vertex_normal_viewspace;
in vec4 vertex_position_viewspace;
in vec2 fs_texture_coordinate;

// Out data
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D tex;

#define PI 3.1415 

float gaussian(float x, float sigma, float mu)
{
  float a = 1.0f / (sigma * sqrt(2.0f * PI));
  float x_minus_b = x - mu;
  return a * exp(-(x_minus_b * x_minus_b) / (2.0f * sigma * sigma));
}

void main()
{
  vec3 n = normalize(vertex_normal_viewspace);
  vec3 l = normalize(vec3(-1));
  vec3 v = normalize(vertex_position_viewspace.xyz - vec3(0.0f));
  vec3 r = reflect(v, n);

  vec3 ambient_color = vec3(0.7f, 0.9f, 1.0f);
  vec3 light_color = vec3(1.0f, 0.9f, 0.7f);

  float light_intensity = 0.7;
  float ambient_intensity = 0.2; 

  vec3 albedo = texture(tex, fs_texture_coordinate).rgb;
  float specularity = 0.1;
  float roughness = 0.01f;
  
  float cosine_term = max(dot(n, -l), 0.0f);
  float theta_term = gaussian(1 - max(dot(r, -l), 0.0f), roughness, 0.0f);

  vec3 ambient = albedo * ambient_color * ambient_intensity;  
  vec3 diffuse = albedo * (1.0f - specularity) * light_color * light_intensity * cosine_term;
  vec3 specular = albedo * specularity * light_color * light_intensity * theta_term;

  color = vec4(diffuse + ambient + specular, 1.0f);
}