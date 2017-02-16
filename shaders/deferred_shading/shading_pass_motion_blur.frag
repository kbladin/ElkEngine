#version 410 core

// Out data
layout(location = 0) out vec4 color;

in vec3 vertex_position_viewspace_unprojected;

// Uniforms
uniform sampler2D irradiance_buffer;
uniform sampler2D position_buffer;
uniform sampler2D albedo_buffer;

uniform mat4 transform_view_to_prev_screen;

uniform ivec2 window_size;


float max_level = 7; // Corresponds to completely out of focus  

vec3 convertIrradiance(vec3 irradiance)
{
  // Reinhard tone mapping
  //vec3 mapped = total_irradiance / (total_irradiance + vec3(1.0));
  // Gamma correction 
  //mapped = pow(mapped, vec3(1.0 / 1.1));

  vec3 v = normalize(vertex_position_viewspace_unprojected);
  float vignetting = dot(-v, vec3(0.0f,0.0f,1.0f));

  return vignetting * irradiance;
}

void main()
{
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
  ivec2 raster_coord = ivec2(gl_FragCoord.xy);

  // Material properties
  vec3 irradiance;// = texelFetch(irradiance_buffer, raster_coord, 0).xyz;
  float alpha = texelFetch(albedo_buffer, raster_coord, 0).a;
  bool infinite_dist = alpha == 0.0f;

  vec3 position;
  vec3 prev_irradiance;
  if (!infinite_dist)
  {
    position = texelFetch(position_buffer, raster_coord, 0).xyz;
  }
  else
  {
    position = vertex_position_viewspace_unprojected * 10000000000.0f;
  }
  vec4 prev_screen = transform_view_to_prev_screen * vec4(position, 1.0);
  prev_screen = prev_screen * (1.0f / prev_screen.w);

  vec2 prev_texture_space = prev_screen.xy * 0.5f + vec2(0.5f);
  vec2 diff_tex_space = prev_texture_space - sample_point_texture_space;
  
  int n_samples = 0;
  float blur_amount = 2.0f;
  for (float t = -1.0f; t < 1.0f; t += 0.2)
  {
    irradiance += textureLod(
      irradiance_buffer, sample_point_texture_space + diff_tex_space * t * blur_amount, 0).rgb;
    n_samples++;
  }
  irradiance /= n_samples;

  // Write color  
  color = vec4(convertIrradiance(irradiance), 1.0f);

  // Write to linear depth buffer
  float max_dist = 1000.0f;
  float depth = infinite_dist ? 1.0f : (-position.z / max_dist);
  gl_FragDepth = depth;
}