#version 330 core

// From previous state
uniform sampler2D positionSampler2D;
uniform sampler2D velocitySampler2D;
// From current state
uniform sampler2D accelerationSampler2D;

uniform float dt;
out vec4 velocity_out;

void main(){
	vec3 a = texelFetch( accelerationSampler2D, ivec2(gl_FragCoord.xy), 0).xyz;
	vec3 v = texelFetch( velocitySampler2D, ivec2(gl_FragCoord.xy), 0).xyz;
	vec3 p = texelFetch( positionSampler2D, ivec2(gl_FragCoord.xy), 0).xyz;


	//for (int i=0; i<5000; i++)
	//{
	//	if (i != gl_FragCoord.x)
	//	{
	//		vec3 diff = texelFetch( positionSampler2D, i, 0).xyz - p;
	//		float dist = length(diff);
	//		if (dist < 0.01)
	//		{
				// Inelastic collision (assuming same radius and same mass)
				//v = (v + texelFetch( velocitySampler2D, i, 0).xyz) / 2;
	//		}
	//	}
	//}

	vec3 delta_v = a * dt;
	vec3 new_vel = v + delta_v;
	
	velocity_out = vec4(new_vel,1);
}
