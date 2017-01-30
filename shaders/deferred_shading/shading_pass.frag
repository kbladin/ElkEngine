#version 410 core

// In data
in vec2 fs_texture_coordinate;

// Out data
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D tex0; // Color
uniform sampler2D tex1; // Position
uniform sampler2D tex2; // Normal

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

vec3 environment(vec3 dir)
{
  return vec3(0.1, 0.1, 0.1);
}

void main()
{
  vec3 albedo = texture(tex0, fs_texture_coordinate).rgb;
  vec3 position = texture(tex1, fs_texture_coordinate).xyz;
  vec3 normal = texture(tex2, fs_texture_coordinate).xyz;

  if (position == vec3(0,0,0))
  {
    color = vec4(environment(vec3(1)), 1);
    return;
  }
    
  vec3 n = normalize(normal);
  vec3 l = normalize(vec3(-1));
  vec3 v = normalize(position - vec3(0.0f));
  vec3 r = reflect(v, n);

  vec3 ambient_color = vec3(0.7f, 0.9f, 1.0f);
  vec3 light_color = vec3(1.0f, 0.9f, 0.7f);

  float light_intensity = 0.7;
  float ambient_intensity = 0.2;

  float roughness = 0.01f;
  float index_of_refraction = 2;
  
  float cos_alpha = max(dot(n, -l), 0.0f);
  float cos_beta = gaussian(1 - max(dot(r, -l), 0.0f), roughness, 0.0f);
  float cos_theta = max(dot(-v, n), 0.0f);

  // Fresnel term
  float R = schlick(1, index_of_refraction, cos_theta);

  //vec3 ambient = albedo * ambient_color * ambient_intensity;  
  vec3 diffuse = albedo * (1.0f - R) * light_color * light_intensity * cos_alpha;
  vec3 specular = R * ((light_color * light_intensity * cos_beta) + environment(r) * (1 - roughness));

  color = vec4(diffuse + specular, 1.0f);
}