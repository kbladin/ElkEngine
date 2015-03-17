#version 330 core


in vec3 vertexPosition;
out vec4 color;

uniform int divisions = 8;
uniform float thickness = 0.03;

void main(){

	float x = ((vertexPosition.x * divisions) - floor(vertexPosition.x * divisions));
	float z = ((vertexPosition.z * divisions) - floor(vertexPosition.z * divisions));

	int drawX = ((x > thickness/2) ? ((x < (1 - thickness/2)) ? 0 : 1) : 1);
	int drawZ = ((z > thickness/2) ? ((z < (1 - thickness/2)) ? 0 : 1) : 1);
    color = vec4(0,0,0, drawX + drawZ);
}
