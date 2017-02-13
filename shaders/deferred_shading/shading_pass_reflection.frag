#version 410 core

// Out data
layout(location = 0) out vec4 final_irradiance;

// Uniforms
uniform sampler2D albedo_buffer; // Albedo
uniform sampler2D position_buffer; // Position
uniform sampler2D normal_buffer; // Normal
uniform sampler2D material_buffer; // Roughness, Dielectric Fresnel term, metalness
uniform sampler2D irradiance_buffer; // Irradiance

uniform mat4 P_frag;

uniform samplerCube cube_map;
uniform int cube_map_size;
uniform mat3 V_inv;









#define point2 vec2
#define point3 vec3

bool traceScreenSpaceRay(point3 csOrig, vec3 csDir, mat4x4 proj,
    sampler2D csZBuffer, vec2 csZBufferSize, float zThickness,
    const bool csZBufferIsHyperbolic, vec3 clipInfo, float nearPlaneZ,
    float stride, float jitter, const float maxSteps, float maxDistance,
    out point2 hitPixel, out int hitLayer, out point3 csHitPoint) {

    // Clip to the near plane
    float rayLength = ((csOrig.z + csDir.z * maxDistance) > nearPlaneZ) ?
            (nearPlaneZ - csOrig.z) / csDir.z : maxDistance;
        point3 csEndPoint = csOrig + csDir * rayLength;
        hitPixel = point2(-1, -1);

    // Project into screen space
    vec4 H0 = proj * vec4(csOrig, 1.0), H1 = proj * vec4(csEndPoint, 1.0);
    float k0 = 1.0 / H0.w, k1 = 1.0 / H1.w;
    point3 Q0 = csOrig * k0, Q1 = csEndPoint * k1;

    // Screen-space endpoints
    point2 P0 = H0.xy * k0, P1 = H1.xy * k1;

    // [ Optionally clip here using listing 4 ]
    P1 += vec2((pow(distance(P0, P1), 2) < 0.0001) ? 0.01 : 0.0);
    vec2 delta = P1 - P0;
    bool permute = false;
    if (abs(delta.x) < abs(delta.y)) {
        permute = true;
        delta = delta.yx; P0 = P0.yx; P1 = P1.yx;
    }
    float stepDir = sign(delta.x), invdx = stepDir / delta.x;

    // Track the derivatives of Q and k.
    vec3 dQ = (Q1 - Q0) * invdx;
    float dk = (k1 - k0) * invdx;
    vec2 dP = vec2(stepDir, delta.y * invdx);
    dP *= stride; dQ *= stride; dk *= stride;
    P0 += dP * jitter; Q0 += dQ * jitter; k0 += dk * jitter;
    float prevZMaxEstimate = csOrig.z;

    // Slide P from P0 to P1, (now-homogeneous) Q from Q0 to Q1, k from k0 to k1
    point3 Q = Q0; float k = k0, stepCount = 0.0, end = P1.x * stepDir;
    for
    (point2 P = P0;
    ((P.x * stepDir) <= end) && (stepCount < maxSteps);
    P += dP, Q.z += dQ.z, k += dk, stepCount += 1.0) {
        // Project back from homogeneous to camera space
        hitPixel = permute ? P.yx : P;
        
        // The depth range that the ray covers within this loop iteration.
        // Assume that the ray is moving in increasing z and swap if backwards.
        float rayZMin = prevZMaxEstimate;
        // Compute the value at 1/2 pixel into the future
        float rayZMax = (dQ.z * 0.5 +Q.z)/(dk*0.5+k);
        prevZMaxEstimate = rayZMax;
        if (rayZMin > rayZMax) { float tmp = rayZMin; rayZMin = rayZMax; rayZMax = tmp; }
        // Camera-space z of the background at each layer (there can be up to 4)
        float sceneZMax = -texelFetch(csZBuffer, ivec2(hitPixel), 0).z;
        float sceneZMin = sceneZMax - zThickness;
        if (((rayZMax >= sceneZMin) && (rayZMin <= sceneZMax)) ||
            (sceneZMax == 0)) {
              hitLayer = 0;
              break; // Breaks out of both loops, since the inner loop is a macro
        }
    } // for each pixel on ray
    // Advance Q based on the number of steps
    Q.xy += dQ.xy * stepCount; csHitPoint = Q * (1.0 / k);
    return all(lessThanEqual(abs(hitPixel - (csZBufferSize * 0.5)),
                         csZBufferSize * 0.5));
} 





















float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float castReflectionRay(vec3 origin, vec3 direction, out vec3 radiance, float roughness)
{
  float step = 0.1;
  float t = 0.0f;
  //origin += step * direction * rand(vec2(direction.x, direction.y));
  vec3 position_view_space = origin;
  vec4 position_clip_space = P_frag * vec4(position_view_space, 1.0f);
  vec3 position_screen_space = position_clip_space.xyz / position_clip_space.w;

  vec3 scale = vec3(P_frag[0][0], P_frag[1][1], P_frag[2][2] + P_frag[3][2]);
  vec3 one_over_scale = vec3(1.0f) / scale;
  
/*
  for (int i = 0; i < 10; i++)
  {
    vec3 position_view_space_prev = position_view_space;

    float x = position_view_space.x;
    float y = position_view_space.y;
    float z = position_view_space.z;

    float x_prim = position_screen_space.x;
    float y_prim = position_screen_space.y;
    float z_prim = position_screen_space.z;

    float a = P_frag[0][0];
    float b = P_frag[1][1];
    float c = P_frag[2][2];
    float d = P_frag[3][2];
    
    vec3 minus_z_vec = vec3(-z, -z, -1);
    vec3 one_over_minus_z_vec = vec3(1.0f) / minus_z_vec;



    //vec3 direction_screen_space = vec3(
    //(a * x * direction.z - a * direction.x * (z)) / ((z) * (z + direction.z)),
    //(b * y * direction.z - b * direction.y * (z)) / ((z) * (z + direction.z)),
    //-d / (z + direction.z) + d / (z));
    //
    ////vec3 direction_screen_space = direction * scale * one_over_minus_z_vec;
    //direction_screen_space /= length(direction_screen_space.xy);
    //vec3 diff_screen_space = direction_screen_space / (720 * 2) * 60;
    //
    //vec3 diff_view_space = vec3(
    //  1.0f / a * (-z_prim * diff_screen_space.x - x_prim * diff_screen_space.z - diff_screen_space.z * diff_screen_space.x),
    //  1.0f / b * (-z_prim * diff_screen_space.y - y_prim * diff_screen_space.z - diff_screen_space.z * diff_screen_space.y),
    //  - d / (z_prim + diff_screen_space.z + c) + d / (z_prim + c)
    //  );
    
    
    
    vec3 diff_view_space = step * direction;
    vec3 diff_screen_space = vec3(
      (a * x * diff_view_space.z - a * diff_view_space.x * z) / (z * (z + diff_view_space.z)),
      (b * y * diff_view_space.z - b * diff_view_space.y * z) / (z * (z + diff_view_space.z)),
      -d / (z + diff_view_space.z) + d / z);
    
  
    position_view_space += diff_view_space;
    position_screen_space += diff_screen_space;
    //vec4 position_clip_space = P_frag * vec4(position_view_space, 1.0f);
    //vec3 position_screen_space = position_clip_space.xyz / position_clip_space.w;
  
    
    vec2 position_texture_space = position_screen_space.xy * 0.5f + vec2(0.5f);
    vec3 position = textureLod(position_buffer, position_texture_space, 0).xyz;
    float alpha = textureLod(albedo_buffer, position_texture_space, 0).a;

    if (position_texture_space.x < 0 || position_texture_space.x > 1 ||
        position_texture_space.y < 0 || position_texture_space.y > 1)
    {
      return 0.0f;
    }
    if (position.z > position_view_space.z &&
        (position.z - position_view_space_prev.z) < 0.2 &&
        alpha >= 0.5f)
    {
      float alpha_remapped = (alpha - 0.5f) * 2.0f;
      radiance += textureLod(irradiance_buffer, position_texture_space, 0).rgb;
      return alpha_remapped;
    }
    t += step;
  }
  return 0.0f;
*/


  for (int i = 0; i < 20; i++)
  {
    vec3 position_view_space_prev = position_view_space;
    position_view_space = origin + t * direction;
    vec4 position_clip_space = P_frag * vec4(position_view_space, 1.0f);
    vec3 position_screen_space = position_clip_space.xyz / position_clip_space.w;
    vec2 position_texture_space = position_screen_space.xy * 0.5f + vec2(0.5f);
    vec3 position = textureLod(position_buffer, position_texture_space, 0).xyz;
    float alpha = textureLod(albedo_buffer, position_texture_space, 0).a;

    if (position_texture_space.x < 0 || position_texture_space.x > 1 ||
        position_texture_space.y < 0 || position_texture_space.y > 1)
    {
      return 0.0f;
    }
    if (position.z > position_view_space.z &&
        //(position.z - position_view_space_prev.z) < 0.2 &&
        alpha >= 0.5f)
    {
      float alpha_remapped = (alpha - 0.5f) * 2.0f;
      radiance += textureLod(irradiance_buffer, position_texture_space, 0).rgb;
      return alpha_remapped;
    }
    t += step;
  }
  return 0.0f;
  

}


float castReflectionRay2(vec3 origin, vec3 direction, out vec3 radiance, float roughness)
{
  float step = 0.1;
  float t = 0.0f;
  //origin += step * direction * rand(vec2(direction.x, direction.y));
  vec3 position_view_space = origin;
  
  vec3 position;
  vec2 position_texture_space;;
  
  bool hit = false;

  for (int i = 0; i < 20; i++)
  {
    step *= 1 + 0.2 * (rand(vec2(direction.x, direction.y)) - 0.5);
    vec3 position_view_space_prev = position_view_space;
    position_view_space = origin + t * direction;
    vec4 position_clip_space = P_frag * vec4(position_view_space, 1.0f);
    vec3 position_screen_space = position_clip_space.xyz / position_clip_space.w;
    position_texture_space = position_screen_space.xy * 0.5f + vec2(0.5f);
    position = textureLod(position_buffer, position_texture_space, 0).xyz;
    float alpha = textureLod(albedo_buffer, position_texture_space, 0).a;

    if (position_texture_space.x < 0 || position_texture_space.x > 1 ||
        position_texture_space.y < 0 || position_texture_space.y > 1)
    {
      return 0.0f;
    }
    if (position.z > position_view_space.z && alpha != 0.0f)
    { // Hit
      hit = true;
      step *= 0.5f;
      t -= step;      
    }
    else if (hit)
    {
      step *= 0.5f;
      t += step;
    }
    else
    {
      step *= 1.5f;
      t += step;
    }
  }

  if (hit && abs(position.z - position_view_space.z) < 0.1)
  {
    float level = clamp(log2(abs(t / (-position.z)) * roughness * 1000), 0, 7);
    float alpha = textureLod(albedo_buffer, position_texture_space, level).a;
    float alpha_remapped = (alpha - 0.5f) * 2.0f;
    radiance = textureLod(irradiance_buffer, position_texture_space, level).rgb;

    return alpha_remapped;
  }
  else
    return 0.0f;
}


vec3 environment(vec3 dir_view_space, float roughness)
{
  float level = clamp(log2(roughness * cube_map_size), 0, 10);
  vec3 dir_world_space = V_inv * dir_view_space;
  vec3 color = textureLod(cube_map, dir_world_space, level).rgb;
  return color;
}

void main()
{
  vec3 specular_radiance_env;
  ivec2 raster_coord = ivec2(gl_FragCoord.xy);
 
  // Material properties
  vec3 irradiance = texelFetch(irradiance_buffer, raster_coord, 0).rgb;
  vec3 position =   texelFetch(position_buffer,   raster_coord, 0).xyz;
  vec4 albedo =     texelFetch(albedo_buffer,     raster_coord, 0);
  
  if (albedo.a > 0.5)
  {
    vec3 normal =     texelFetch(normal_buffer,   raster_coord, 0).xyz;
    float roughness = texelFetch(material_buffer, raster_coord, 0).x;
    float R =         texelFetch(material_buffer, raster_coord, 0).y;
    float metalness = texelFetch(material_buffer, raster_coord, 0).z;

    // Useful vectors
    vec3 n = normalize(normal);
    vec3 v = normalize(position - vec3(0.0f));
    vec3 r = reflect(v, n);

    // Form factors
    float cos_alpha = max(dot(-v, n), 0.0f);

    // BRDFs
    float BRDF_specular_times_cos_theta_at_reflection = 1.0f;

    // Irradiance measured in Watts per square meter
    // [M * L^2 * T^-3] * [Sr^-1] * [L^-2] = [M * Sr^-1 * T^-3]
    // Rendering equation over whole hemisphere
    float irradiance_specular_environment = 1.0f * BRDF_specular_times_cos_theta_at_reflection;

    vec3 radiance_reflection = vec3(0);
    float hit = 0;// castReflectionRay2(position + n * 0.01f, r, radiance_reflection, roughness);
    



    // Fade out reflections toward camera
    hit *= 1 - cos_alpha;

    // Different Frenel depending on if the material is metal or dielectric
    vec3  R_metal = (albedo.rgb + (vec3(1.0f) - albedo.rgb) * vec3(R));
    vec3  R_specular = vec3(R * (1.0f - metalness)) + R_metal * metalness;

    // Filter radiance through colors and material  
    specular_radiance_env = R_specular * (environment(r, roughness) * (1 - hit) + hit * radiance_reflection) * irradiance_specular_environment;
  }
  // Add to final radiance
  final_irradiance = vec4(max(specular_radiance_env + irradiance, 0.0f), 1.0f);
}