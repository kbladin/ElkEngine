#version 410 core

// In data
in vec4 vertex_position_viewspace;
in vec3 vertex_normal_viewspace;
in vec3 vertex_tangent_viewspace;
in vec2 fs_texture_coordinate;

// Out data
layout(location = 0) out vec4 albedo;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec3 material; // Roughness, Fresnel Term, metalness

// Uniforms
uniform sampler2D albedo_texture;
uniform sampler2D roughness_texture;
uniform sampler2D R0_texture;
uniform sampler2D metalness_texture;
uniform sampler2D normal_texture;

// R0 is calculated from IOR as so:
// R0 = pow((n1 - n2) / (n1 + n2), 2)
float schlick(float R0, float cos_theta)
{
  float R = R0 + (1 - R0) * pow((1 - cos_theta), 5);
  return R;
}

float roughSchlick2(float R0, float cos_theta, float roughness)
{
  float area_under_curve = 1.0 / 6.0 * (5.0 * R0 + 1.0);
  float new_area_under_curve = 1.0 / (6.0 * roughness + 6.0) * (5.0 * R0 + 1.0);

  return schlick(R0, cos_theta) /
    (1 + roughness) + (area_under_curve - new_area_under_curve);
}

float remapRoughness(float x)
{
  return 2.0f * (1.0f / (1.0f - 0.5f + 0.001f) - 1.0f) * (pow(x, 2)) + 0.001f;
}

void main()
{
  position = vertex_position_viewspace.xyz;

  vec3 sampled_normal = texture(normal_texture, fs_texture_coordinate).xyz;
  normal = normalize(vertex_normal_viewspace); 

  if (length(sampled_normal) != 0.0f)
  {
    vec3 tangent = normalize(vertex_tangent_viewspace); 
    sampled_normal = (2.0f * sampled_normal) - vec3(1.0f);
    vec3 bitangent = cross(normal, tangent);

    normal =
      tangent * sampled_normal.x +
      bitangent * sampled_normal.y +
      normal * sampled_normal.z;
  }

        albedo =      texture(albedo_texture,     fs_texture_coordinate);
  float roughness =   texture(roughness_texture,  fs_texture_coordinate).r;
  float R0 =          0.04;//texture(R0_texture,         fs_texture_coordinate).r;
  float metalness =   texture(metalness_texture,  fs_texture_coordinate).r;

  // Calculate dielctric Fresnel term
  vec3 v = normalize(position);
  float cos_theta = max(dot(-v, normal),  0.0f);
  float remapped_roughness = remapRoughness(roughness);
  float fresnel_term = roughSchlick2(R0, cos_theta, remapped_roughness);

  material = vec3(roughness + 0.01, fresnel_term, metalness);

  // Write to linear depth buffer
  float max_dist = 1000.0f;
  float depth = (-position.z / max_dist);
  gl_FragDepth = depth;
}