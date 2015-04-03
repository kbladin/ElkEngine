#version 330 core

// From previous state
uniform sampler1D positionSampler1D;
// From current state
uniform sampler1D velocitySampler1D;
uniform sampler1D accelerationSampler1D;

uniform float dt;
out vec4 position_out;

void main(){
	vec3 a = texelFetch( accelerationSampler1D, int(gl_FragCoord.x), 0).xyz;
	vec3 v = texelFetch( velocitySampler1D, int(gl_FragCoord.x), 0).xyz;
	vec3 p = texelFetch( positionSampler1D, int(gl_FragCoord.x), 0).xyz;

	vec3 delta_p = v * dt;
	vec3 new_pos = p + delta_p;

    position_out = vec4(new_pos,1);
}
