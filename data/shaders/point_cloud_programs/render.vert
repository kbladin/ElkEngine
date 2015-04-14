#version 330 core

layout(location = 0) in vec2 index;

// Values that stay constant for the whole mesh.
uniform sampler2D accelerationSampler2D;
uniform sampler2D velocitySampler2D;
uniform sampler2D positionSampler2D;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

//out vec2 index_frag;

out vec3 a;
out vec3 v;
out vec3 p;

void main(){
	a = texelFetch( accelerationSampler2D, ivec2(index), 0).xyz;
	v = texelFetch( velocitySampler2D, ivec2(index), 0).xyz;
	p = texelFetch( positionSampler2D, ivec2(index), 0).xyz;
	vec4 pos_cam_space = V * M * vec4(p ,1);
	gl_Position = P * pos_cam_space;
	gl_PointSize = 1 / (-pos_cam_space.z) * 10;
}