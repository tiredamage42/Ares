#type compute
#version 450 core

// converts equirectangular (lat-long) projection texture into
// a proper cubemap


layout(binding = 0) uniform sampler2D u_EquirectangularTex;
layout(binding = 0, rgba16f) restrict writeonly uniform imageCube o_Cubemap;

// calcualte normalized sampling direction vector
// based on current fragment coordinates (gl_GlobalInvocationID.xyz).
// this is essentially "inverse-sampling": we reconstruct what the
// sampling vector would be if we wanted it to "hit"
// this particular fragment in a cubemap.

// see: opengl core profile specs, section 8.13

vec3 GetSamplingVector()
{
	vec2 st = gl_GlobalInvocationID.xy / vec2(imageSize(o_Cubemap));
	vec2 uv = 2.0 * vec2(st.x, 1.0 - st.y) - vec2(1.0);

	vec3 ret;
	// select vector based on cubemap face index.
	// sadly switch doesnt seem to work, at least on nvidia
	if		(gl_GlobalInvocationID.z == 0) ret = vec3(1.0,  uv.y, -uv.x);
	else if (gl_GlobalInvocationID.z == 1) ret = vec3(-1.0, uv.y,  uv.x);
	else if (gl_GlobalInvocationID.z == 2) ret = vec3(uv.x, 1.0, -uv.y);
	else if (gl_GlobalInvocationID.z == 3) ret = vec3(uv.x, -1.0, uv.y);
	else if (gl_GlobalInvocationID.z == 4) ret = vec3(uv.x, uv.y, 1.0);
	else if (gl_GlobalInvocationID.z == 5) ret = vec3(-uv.x, uv.y, -1.0);
	return normalize(ret);
}


const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

//const float PI = 3.141592;
//const float TwoPI = 2 * PI;
//vec2 SampleSphericalMap(vec3 v)
//{
//	// convert cartesian direction vector to spherical coordinates
//	return vec2(
//		atan(v.z, v.x) / TwoPI + 0.5, 
//		1.0 - acos(v.y) / PI
//	);
//}


layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main()
{
	vec3 v = GetSamplingVector();
	vec2 uv = SampleSphericalMap(v);

	// sample equirectangular texture
	vec4 color = texture(u_EquirectangularTex, uv);
	// write out color to output cubemap
	imageStore(o_Cubemap, ivec3(gl_GlobalInvocationID), color);
}