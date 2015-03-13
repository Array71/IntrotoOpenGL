#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec4 Normal; 
layout(location=2) in vec2 TexCoord; 
layout(location=3) in vec4 Tangent; 
layout(location=4) in vec4 Weights; 
layout(location=5) in vec4 Indices; 

out vec4 vNormal; 
out vec4 vPosition; 
out vec2 vTexCoord; 
out vec3 vTangent; 
out vec3 vBiTangent; 

uniform mat4 ProjectionView; 

const int MAX_BONES = 128; 
uniform mat4 Bones[MAX_BONES]; 

void main() { 
	vPosition = Position; 
	vNormal = Normal; 
	vTexCoord = TexCoord; 
	vTangent = Tangent.xyz; 
	vBiTangent = cross(Normal.xyz, Tangent.xyz); 
	ivec4 index = ivec4(Indices); 
	
	vec4 P  = Bones[index.x] * Position * Weights.x; 
		 P += Bones[index.y] * Position * Weights.y; 
		 P += Bones[index.z] * Position * Weights.z; 
		 P += Bones[index.w] * Position * Weights.w; 
		 
	vec4 N  = mat4(inverse(mat4(transpose(Bones[index.x])))) * Normal * Weights.x; 
		 N += mat4(inverse(mat4(transpose(Bones[index.y])))) * Normal * Weights.y; 
		 N += mat4(inverse(mat4(transpose(Bones[index.z])))) * Normal * Weights.z; 
		 N += mat4(inverse(mat4(transpose(Bones[index.w])))) * Normal * Weights.w; 
		 
	vNormal = N; 
	gl_Position = ProjectionView * P; 
}