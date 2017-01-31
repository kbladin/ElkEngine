#version 410 core

// In data
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texture_coordinate;

// Uniform data
// Transform matrices
uniform mat4 M = mat4(1.0f);
uniform mat4 V = mat4(1.0f);
uniform mat4 P = mat4(1.0f);

void main()
{
	// Set camera position
	vec4 vertex_position_viewspace = V * M * vec4(position ,1);

	// Position and size of point
	gl_Position = P * vertex_position_viewspace;
}