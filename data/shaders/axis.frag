#version 330 core

in vec3 interpolatedColor;

out vec4 color;

void main(){
    color.rgb = interpolatedColor * 100;
    color.a = 1;
}
