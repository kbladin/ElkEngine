#version 330 core

// All attribures are from previous state
uniform sampler2D positionSampler2D;
uniform sampler2D velocitySampler2D;
uniform sampler2D accelerationSampler2D;

uniform float dt;
uniform int size;
out vec4 acceleration_out;

void main(){
	vec3 a = texelFetch( accelerationSampler2D, ivec2(gl_FragCoord.xy), 0).xyz;
	vec3 v = texelFetch( velocitySampler2D, ivec2(gl_FragCoord.xy), 0).xyz;
	vec3 p = texelFetch( positionSampler2D, ivec2(gl_FragCoord.xy), 0).xyz;

	float G = 0.00000000005 * size;

	float m = 100;
	// For every mass unit there is 5.4 dark matter units
	float m_dark = pow(size, 2) * 5.4;

	vec3 center_of_mass = vec3(0,0,0);

	vec3 F = vec3(0,0,0);
	for (int i=0; i<size; i++)
	{
		for (int j=0; j<size; j++)
		{
			if (ivec2(i,0) != gl_FragCoord.xy)
			{
				vec3 diff = texelFetch( positionSampler2D, ivec2(i,j), 0).xyz - p;
				float dist = length(diff);
				if (dist > 0.01)
				{
					vec3 gravity = m * m / pow(dist,2) * G * normalize(diff);
					F += gravity;
				}
				/*
				else if (dist > 0.005)
				{
					vec3 v_diff = texelFetch( velocitySampler2D, ivec2(i,j), 0).xyz - v;
					float speed_diff = length(v_diff);
					if (speed_diff > 0.01)
					{
						float v_diff_collision_dir = dot(v_diff, normalize(diff));
						F += v_diff_collision_dir * normalize(diff) * 0.01;
					}
					//vec3 v_diff = texelFetch( velocitySampler2D, ivec2(i,j), 0).xyz - v;
					//float v_diff_collision_dir = dot(v_diff, normalize(diff));
					//F += v_diff_collision_dir * normalize(diff);
				}
				
				else if (dist < 0.005)
				{
					vec3 anti_gravity = - normalize(diff) * 3;
					F += anti_gravity;
				}*/
			}
			//center_of_mass += texelFetch( positionSampler2D, ivec2(i,j), 0).xyz;
		}
	}
	/*
	vec3 center_diff = center_of_mass - p;
	float center_dist = length(center_diff);
	if (center_dist > 0.03)
	{
		F += m * m_dark / pow(center_dist,2) * G * normalize(center_diff);
	}
*/
	a = F/m;
    acceleration_out = vec4(a.xyz,1);
}
