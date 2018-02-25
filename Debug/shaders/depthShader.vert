#version 410

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

layout (location = 10) in mat4 _InstancedModelMatrix;

out vec4 cameraSpacePosition;

// Object Matrices
uniform mat4 _View;
uniform mat4 _Projection;

// Unused
uniform mat4 _Model;


void main(){
	cameraSpacePosition = _View * _InstancedModelMatrix * vec4(vertexPosition,1);
	gl_Position =  _Projection * _View * _InstancedModelMatrix * vec4(vertexPosition,1);
}