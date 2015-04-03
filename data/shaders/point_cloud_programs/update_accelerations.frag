#version 330 core

// All attribures are from previous state
uniform sampler1D positionSampler1D;
uniform sampler1D velocitySampler1D;
uniform sampler1D accelerationSampler1D;

uniform float dt;
out vec4 acceleration_out;

void main(){
	vec3 a = texelFetch( accelerationSampler1D, int(gl_FragCoord.x), 0).xyz;
	vec3 v = texelFetch( velocitySampler1D, int(gl_FragCoord.x), 0).xyz;
	vec3 p = texelFetch( positionSampler1D, int(gl_FragCoord.x), 0).xyz;

	float m = 1;
	// For every mass unit there is 5.4 dark matter units
	float m_dark = 5000 * 5.4;

	vec3 F = vec3(0,0,0);
	//for (int i=0; i<5000; i++)
	//{
	//	if (i != gl_FragCoord.x)
	//	{
	//		vec3 diff = texelFetch( positionSampler1D, i, 0).xyz - p;
	//		float dist = length(diff);
	//		if (dist > 0.01)
	//		{
	//			vec3 gravity = m * m / pow(dist,2) * 0.0000002 * normalize(diff);
	//			F += gravity;
	//		}
	//	}
	//}
	
	vec3 center_diff = vec3(0,0,0) - p;
	float center_dist = length(center_diff);
	if (center_dist > 0.1)
	{
		vec3 gravity = m * m_dark / pow(center_dist,2) * 0.0000002 * normalize(center_diff);
		F += gravity;
	}

	a = F/m;

    acceleration_out = vec4(a.xyz,1);
}
