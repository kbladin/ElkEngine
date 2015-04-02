#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in float index;


// Values that stay constant for the whole mesh.
uniform sampler1D textureSampler1D;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main(){
	//vertexPosition = vertexPosition_modelspace;
	//gl_Position = P * V * M * vec4(vertexPosition_modelspace + texelFetch( textureSampler1D, index, 0).rgb,1);
	gl_Position = P * V * M * vec4(vertexPosition_modelspace + texelFetch( textureSampler1D, int(index), 0).rgb,1);
}