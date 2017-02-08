#version 410 core

// Out data
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D irradiance_buffer; // Irradiance

void main()
{
  ivec2 raster_coord = ivec2(gl_FragCoord.xy);
 
  // Material properties
  vec3 irradiance = texelFetch(irradiance_buffer, raster_coord, 0).rgb;
  color = vec4(irradiance, 1.0f);
}