#version 330 core

layout(location = 0) in float index;

// Values that stay constant for the whole mesh.
uniform sampler1D accelerationSampler1D;
uniform sampler1D velocitySampler1D;
uniform sampler1D positionSampler1D;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out float index_frag;

void main(){
	index_frag = index;
	gl_Position = P * V * M * vec4(texelFetch( positionSampler1D, int(index), 0).rgb,1);
}