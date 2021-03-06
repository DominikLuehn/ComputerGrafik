#version 330 core

layout(location = 0) in vec3 vertex_position; 
layout(location = 1) in vec3 aNormal; 
layout(location = 2) in vec2 aTexCoord;

out vec4 PosLightSpace;
out vec3 Normal; 
out vec3 Position;
out vec2 TexCoord;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main(){ 
	TexCoord = aTexCoord;
	Normal = mat3(transpose(inverse(transform))) * aNormal; 
	Position = vec3(transform * vec4(vertex_position, 1.0));
	PosLightSpace = lightSpaceMatrix * vec4(Position, 1.0);
	gl_Position = projection * view * transform * vec4(Position, 1.0); 
}