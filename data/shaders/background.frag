#version 330 core

in vec3 vertexPosition;

out vec4 color;

uniform vec3 material_diffiseColor;

void main(){
	float w = 0.4;
	float i = 0.6;
    color.rgb = ((vec3(vertexPosition.y + 1, vertexPosition.y + 1, 2) / 2) * w + vec3(1,1,1) * (1-w)) * i;
    color.a = 1;
}
