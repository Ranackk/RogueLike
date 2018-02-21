#version 410

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

out vec2 uv;

// Unused
uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

uniform vec3 _ParticleCenter;		// World space


void main(){
	vec3 cameraRight_worldspace = {_View[0][0], _View[1][0], _View[2][0]};
    vec3 cameraUp_worldspace = {_View[0][1], _View[1][1], _View[2][1]};
	vec4 vertex_worldspace = _View * _Model * vec4(vertexPosition,1);
	vec3 vertexPosition_worldspace = _ParticleCenter + cameraRight_worldspace * vertex_worldspace.x * 1  + cameraUp_worldspace * vertex_worldspace.y * 1;
									
	gl_Position = vec4(vertexPosition_worldspace,0);
	uv = vertexUV;
}