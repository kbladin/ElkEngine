#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
//layout(location = 1) in vec3 vertexColor;

out vec3 interpolatedColor;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main(){
	gl_Position = P * V * M * vec4(vertexPosition_modelspace,1);
	interpolatedColor = vertexPosition_modelspace;
}