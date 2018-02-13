#version 410

in vec2 uv;

out vec4 color;

uniform sampler2D _Texture;


void main(){
	vec4 bg = texture(_Texture, uv);
	color = bg;

	// If the frag is not part of the ui, discard it
	if (color.a < 0.9) discard; 
	gl_FragDepth = 0;
}