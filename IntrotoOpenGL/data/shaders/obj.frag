#version 410

in vec4 vNormal; 
in vec4 vPosition; 
in vec2 vTexCoord; 

out vec4 FragColor; 

uniform vec3 LightDirection; 
uniform vec3 LightColour; 
uniform vec3 CameraPosition; 
uniform float SpecularPower; 
uniform sampler2D Diffuse; 

void main() { 
	float d = max(0, dot(normalize(vNormal.xyz), LightDirection)); 
	vec3 E = normalize(CameraPosition - vPosition.xyz); 
	vec3 R = reflect(-LightDirection, vNormal.xyz); 
	float s = max(0, dot(E, R)); 
	s = pow( s, SpecularPower); 
	FragColor = texture(Diffuse, vTexCoord) * vec4(LightColour * d + LightColour * s ,1); 
}