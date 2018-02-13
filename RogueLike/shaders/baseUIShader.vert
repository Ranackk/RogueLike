#version 410

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

out vec2 uv;

// Unused
uniform mat4 _Model;


void main(){
	gl_Position = _Model * vec4(vertexPosition,1);

	uv = vertexUV;
}