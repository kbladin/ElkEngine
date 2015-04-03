#version 330 core

out vec4 color;

uniform sampler1D accelerationSampler1D;
uniform sampler1D velocitySampler1D;
uniform sampler1D positionSampler1D;

in float index_frag;

void main(){
	vec3 v = texelFetch( velocitySampler1D, int(index_frag), 0).rgb;
	float l = length(v);
	float norm = 0.5;

	vec3 c = vec3(1,1,1) * l / norm;

	color = vec4(c,1);
}
