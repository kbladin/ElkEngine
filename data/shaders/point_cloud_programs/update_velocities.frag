#version 330 core

// From previous state
uniform sampler1D positionSampler1D;
uniform sampler1D velocitySampler1D;
// From current state
uniform sampler1D accelerationSampler1D;

uniform float dt;
out vec4 velocity_out;

void main(){
	vec3 a = texelFetch( accelerationSampler1D, int(gl_FragCoord.x), 0).xyz;
	vec3 v = texelFetch( velocitySampler1D, int(gl_FragCoord.x), 0).xyz;
	vec3 p = texelFetch( positionSampler1D, int(gl_FragCoord.x), 0).xyz;


	//for (int i=0; i<5000; i++)
	//{
	//	if (i != gl_FragCoord.x)
	//	{
	//		vec3 diff = texelFetch( positionSampler1D, i, 0).xyz - p;
	//		float dist = length(diff);
	//		if (dist < 0.01)
	//		{
				// Inelastic collision (assuming same radius and same mass)
				//v = (v + texelFetch( velocitySampler1D, i, 0).xyz) / 2;
	//		}
	//	}
	//}

	vec3 delta_v = a * dt;
	vec3 new_vel = v + delta_v;
	
	velocity_out = vec4(new_vel,1);
}
