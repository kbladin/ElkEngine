#version 410 core

// Out data
layout(location = 0) out vec4 color;

// Uniforms
uniform sampler2D color_buffer;
uniform ivec2 window_size;

void main()
{
  vec2 sample_point_texture_space = gl_FragCoord.xy / window_size;
 
 	// Right now we don't have acces to the view direction..
  float vignetting = 1.0;// dot(-v, vec3(0.0f,0.0f,1.0f));
  // Material properties
  color = vec4(vignetting * texture(color_buffer, sample_point_texture_space).rgb, 1.0f);
}