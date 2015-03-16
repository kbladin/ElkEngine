#version 330 core

in vec3 Normal_cameraspace;

out vec3 color;

void main(){
    color = Normal_cameraspace;
}
