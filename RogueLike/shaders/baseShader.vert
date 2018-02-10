#version 410

// "0" means 0 in glVertexAttribPointer, all thos 3 variables are in modelspace
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

layout (location = 10) in mat4 _InstancedModelMatrix;

out vec2 uv;
out vec4 posWorldSpace;
out vec4 posModelSpace;
out vec4 clipPosition;
out vec4 normalWorldSpace;
 
uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

uniform int _UseInstancing;


void main(){
	mat4 modelMatrix;
	if (_UseInstancing > 0) modelMatrix = _InstancedModelMatrix;
	else modelMatrix = _Model;

	mat4 mvp = _Projection * _View * modelMatrix;
	clipPosition = mvp * vec4(vertexPosition.xyz, 1);
	gl_Position = clipPosition;

	posModelSpace = vec4(vertexPosition.xyz,1);

	posWorldSpace = modelMatrix * vec4(vertexPosition.xyz,1);
	posWorldSpace /= posWorldSpace.w;

	uv = vertexUV;
	normalWorldSpace = modelMatrix * vec4(vertexNormal, 0);


}