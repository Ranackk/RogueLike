#version 410

in vec4 cameraSpacePosition;

uniform vec2 _ClippingPlane;

void main(){
	/* Take the frags position in cameraSpace, devided by its w coordinate, and make it linear between near and far plane from 0 to 1 */
	gl_FragDepth = clamp((( length(cameraSpacePosition.xyz) / cameraSpacePosition.w) - _ClippingPlane.x) / (_ClippingPlane.y - _ClippingPlane.x), 0, 1);
}