#version 330 core

in vec3 Normal_viewspace;
in vec3 vertexPosition_viewspace;
in vec3 lightPosition_viewspace;

out vec4 color;

uniform float lightIntensity;
uniform vec3 lightColor;

uniform vec3 material_diffiseColor;
uniform vec3 material_specularColor;
uniform float material_specularity;
uniform int material_shinyness;

void main(){
	vec3 l = normalize(vertexPosition_viewspace - lightPosition_viewspace);
	vec3 v = normalize(vertexPosition_viewspace);
	vec3 n = normalize(Normal_viewspace);
	vec3 r = reflect(l,n);

	float cosTheta = clamp(dot(-l,n),0,1);
	float cosAlpha = clamp(dot(v,-r),0,1);

	float distance = length(vertexPosition_viewspace - lightPosition_viewspace);
	float invDistanceSquare = 1 / pow(distance, 2);

	vec3 underLight = clamp(dot(-vec3(0,1,0),n),0,1) * material_diffiseColor * 0.2;

	vec3 ambient = vec3(0.2,0.2,0.2) * material_diffiseColor;
	vec3 diffuse =
		lightIntensity * 
		cosTheta * 
		invDistanceSquare *
		lightColor * 
		material_diffiseColor;
	
	vec3 specular =
		lightIntensity *
		material_specularity *
		pow(cosAlpha, material_shinyness) *
		invDistanceSquare *
		lightColor *
		material_specularColor;

    color.rgb = ambient + diffuse + specular + underLight;
    color.a = 1;
}
