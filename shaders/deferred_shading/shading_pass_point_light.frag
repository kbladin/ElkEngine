#version 410 core

struct PointLightSource
{
  vec3  position;
  vec3  color; // Works as a filter
  float radiant_flux; // Given in Watt [M * L^2 * T^-3]
};

// Out data
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D tex0; // Albedo
uniform sampler2D tex1; // Position
uniform sampler2D tex2; // Normal
uniform sampler2D tex3; // Roughness

uniform PointLightSource light_source;

uniform ivec2 window_size;
uniform mat4 P_frag;

uniform samplerCube cube_map;

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

// Approximation of error function with maximum error: 2.5×10^−5
float erf(float x)
{
  float p = 0.47047;
  float a1 = 0.3480242;
  float a2 = -0.0958798;
  float a3 = 0.7478556;
  float t = 1.0 / (1.0 + p * x);
  
  return 1 - (a1 * t + a2 * pow(t,2) + a3 * pow(t,3)) * exp(-pow(x,2));
}

float roughSchlick(float n1, float n2, float cos_theta, float roughness)
{
  float x = cos_theta;
  float R0 = pow((n1 - n2) / (n1 + n2), 2);
  float a = 1.0f / (roughness * sqrt(2.0f * PI));
  float c = roughness;
  float sqrt_2 = sqrt(2);
  float sqrt_pi = sqrt(PI);
  float s2c = sqrt_2 * c;

  float b = 1.0f / s2c;

  // Integration from 0 to 1
  float I1 = a * R0 * 1.0f / 2.0f * sqrt_pi * s2c * erf(1.0f / s2c);
  float I2 = (a * (1.0f-R0)) * s2c * (-1.0f) / (8.0f * pow(b, 5.0f)) * (exp(-pow(b,2.0f)) * (sqrt_pi * b * exp(pow(b,2.0f)) * (x - 1.0f) * erf(b) * ( 4.0f * pow(b,4.0f) * pow(x-1.0f, 4.0f) + 20.0f * pow(b,2.0f) * pow(x-1.0f, 2.0f) + 15.0f) - 2.0f * (2.0f*pow(b,4.0f) * (5.0f*pow(x,4.0f) - 10.0f*pow(x,3.0f) + 10.0f*pow(x,2.0f) - 5.0f*x + 1.0f ) + pow(b,2.0f) * (20.0f*pow(x,2.0f) -25.0f*x + 9.0f) + 4.0f )) + 2.0f * (10.0f*pow(b,4.0f) * pow(x-1.0f,4.0f) + 20.0f*pow(b,2.0f)*pow(x-1.0f,2.0f) + 4.0f ) );


  // Integration from -inf to inf
  //float I1 = a * c * R0 * sqrt_2 * sqrt_pi;
  //float I2 = a * (1 - R0) * s2c * 1/4.0 * sqrt_pi * (1 - x) * (15 * pow(s2c, 4) + 20 * pow(s2c, 2) * pow(1 - x, 2) + 4 * pow(1 - x, 4) );

  return I1 + I2;
}


float roughSchlick2(float n1, float n2, float cos_theta, float roughness)
{
  float R0 = pow((n1 - n2) / (n1 + n2), 2);
  float area_under_curve = 1.0 / 6.0 * (5.0 * R0 + 1.0);
  float new_area_under_curve = 1.0 / (6.0 * roughness + 6.0) * (5.0 * R0 + 1.0);

  return schlick(n1, n2, cos_theta) / (1 + roughness) + (area_under_curve - new_area_under_curve);
}

vec3 gammaCorrection(vec3 v, float gamma)
{
  return vec3(pow(v.x, gamma), pow(v.y, gamma), pow(v.z, gamma));
}

float castShadowRay(vec3 origin, vec3 direction)
{
  float hit = 0.0f;
  float step = 0.1f;
  float t = 0.0f;
  for (int i = 0; i < 10; i++)
  {
    vec3 position_view_space = origin + t * direction;
    vec4 position_screen_space = P_frag * vec4(position_view_space, 1.0f);
    position_screen_space /= position_screen_space.w;
    vec2 position_texture_space = position_screen_space.xy * 0.5f + vec2(0.5f);
    vec3 position = texture(tex1, position_texture_space).xyz;
    float alpha = texture(tex0, position_texture_space).a;

    hit += (position.z > position_view_space.z ? 1.0f : 0.0f) * alpha;
      
    t += step;
  }
  return 1.0f - clamp(hit, 0.0f, 1.0f);
}

void main()
{
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
  
  // Material properties
  vec4 albedo =     texture(tex0, sample_point_texture_space);
  vec3 position =   texture(tex1, sample_point_texture_space).xyz;
  vec3 normal =     texture(tex2, sample_point_texture_space).xyz;
  float roughness = texture(tex3, sample_point_texture_space).x;
  float index_of_refraction = texture(tex3, sample_point_texture_space).y;

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

  // Fresnel term
  //float R = schlick(1, index_of_refraction, cos_alpha);
  float R = roughSchlick2(1, index_of_refraction, cos_alpha, roughness);

  // BRDFs
  float BRDF_diffuse = 1.0;
  float BRDF_specular_times_cos_theta = gaussian(1 - cos_beta, roughness, 0.0f);

  // Irradiance measured in Watts per square meter
  // [M * L^2 * T^-3] * [Sr^-1] * [L^-2] = [M * Sr^-1 * T^-3]
  // Rendering equation over whole hemisphere
  float light_source_radiance = light_source.radiant_flux * inv_dist_square;
  float irradiance_diffuse =  light_source_radiance * BRDF_diffuse      * cos_theta * 2 * PI;
  float irradiance_specular = light_source_radiance * BRDF_specular_times_cos_theta * 2 * PI;

  // Filter radiance through colors and material
  vec3 diffuse_radiance = albedo.rgb * (1.0f - R) * light_source.color * irradiance_diffuse;
  vec3 specular_radiance =                 R  * light_source.color * irradiance_specular;

  // Add to final radiance
  color = vec4((diffuse_radiance + specular_radiance) * albedo.a * castShadowRay(position + n * 0.01f, -l), 1.0f);
}