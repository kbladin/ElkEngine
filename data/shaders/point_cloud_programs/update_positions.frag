#version 330 core

// From previous state
uniform sampler2D positionSampler2D;
// From current state
uniform sampler2D velocitySampler2D;
uniform sampler2D accelerationSampler2D;

uniform float dt;
out vec4 position_out;

void main(){
	vec3 a = texelFetch( accelerationSampler2D, ivec2(gl_FragCoord.xy), 0).xyz;
	vec3 v = texelFetch( velocitySampler2D, ivec2(gl_FragCoord.xy), 0).xyz;
	vec3 p = texelFetch( positionSampler2D, ivec2(gl_FragCoord.xy), 0).xyz;

	vec3 delta_p = v * dt;
	vec3 new_pos = p + delta_p;

    position_out = vec4(new_pos,1);
}
