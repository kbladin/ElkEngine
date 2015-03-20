#version 330 core

out vec4 color;

uniform vec3 material_diffiseColor;

void main(){
    color.rgb = material_diffiseColor;
    color.a = 1;
}
