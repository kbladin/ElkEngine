#version 330 core

uniform sampler1D textureSampler1D;
uniform float dt;
out vec4 color;

void main(){
	vec3 velocity = vec3(1,1,1) * 0.01;
	vec3 delta_position = velocity * dt;
    color = vec4(texelFetch( textureSampler1D, int(gl_FragCoord.x), 0).rgb + delta_position,1);
}
