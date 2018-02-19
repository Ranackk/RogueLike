#version 410

in vec2 uv;

out vec4 color;

uniform sampler2D _BackgroundTexture;
uniform sampler2D _FillTexture;
uniform sampler2D _GradientTexture;

uniform float _FillAmount;

void main(){
	vec4 bg = texture(_BackgroundTexture, uv);
	vec4 fill = texture(_FillTexture, uv);
	vec4 grad = texture(_GradientTexture, uv);

	if ( grad.r <= 1 - _FillAmount){
		color = bg;
	}
	else{
		color = bg + fill;
	}
	// If the frag is not part of the healthbar, discard it
	if (color.a < 0.9) discard; 
	gl_FragDepth = 0;
}