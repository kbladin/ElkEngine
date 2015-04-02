#version 330 core

layout(location = 0) in float index;

// Values that stay constant for the whole mesh.
uniform sampler1D textureSampler1D;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main(){
	gl_Position = P * V * M * vec4(texelFetch( textureSampler1D, int(index), 0).rgb,1);
}