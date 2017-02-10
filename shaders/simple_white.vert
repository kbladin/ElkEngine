#version 410 core

// In data
layout(location = 0) in vec3 position;

// Out data
out vec3 position_viewspace_vert;

// Uniform data
// Transform matrices
uniform mat4 M = mat4(1.0f);
uniform mat4 V = mat4(1.0f);
uniform mat4 P = mat4(1.0f);

void main()
{
	// Set camera position
	vec4 vertex_position_viewspace = V * M * vec4(position ,1);
	position_viewspace_vert = vertex_position_viewspace.xyz;

	// Position and size of point
	gl_Position = P * vertex_position_viewspace;
}