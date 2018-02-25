#version 410
#define MAX_LIGHT_COUNT 64

in vec2 uv;
in vec4 posWorldSpace;
in vec4 normalWorldSpace;

in vec4 posModelSpace;
in vec4 clipPosition;

out vec4 color;

uniform sampler2D _Texture;

uniform int _LightCount;
uniform float[MAX_LIGHT_COUNT] _LightRanges;
uniform float[MAX_LIGHT_COUNT] _LightActives;
uniform vec3[MAX_LIGHT_COUNT] _LightPositions;
uniform vec4[MAX_LIGHT_COUNT] _LightColors;
uniform samplerCubeArrayShadow _LightStaticShadowMaps;
uniform samplerCubeArrayShadow _LightDynamicShadowMaps;

uniform samplerCube _Skybox;
uniform vec4 _Diffuse;

uniform vec2 _ShadowClippingPlane;
uniform vec2 _UvOffset;

vec4 calculateLight(int lightID){
	float lRange = _LightRanges[lightID];
	vec3 lPosition = _LightPositions[lightID];
	vec4 lColor = _LightColors[lightID];

	float distance = length(lPosition - posWorldSpace.xyz);
	float falloff = clamp(lRange * 1 / (distance * distance), 0, 1);
	vec3 lightDirection = normalize(posWorldSpace.xyz - lPosition);
	float lambert = clamp(dot(normalWorldSpace.xyz, - lightDirection), 0, 1);
	
	vec4 toLightVector = -(posWorldSpace - vec4(lPosition.xyz, 1));
	toLightVector.x *= -1;

	float lightDistance01 = clamp(((length(toLightVector.xyz)) - _ShadowClippingPlane.x) / (_ShadowClippingPlane.y - _ShadowClippingPlane.x), 0, 1) - 0.005; // - 0.007 

	float cubeMapShadowSampleStat = texture(_LightStaticShadowMaps, vec4(toLightVector.xyz, lightID), lightDistance01);
	float cubeMapShadowSampleDynm = texture(_LightDynamicShadowMaps, vec4(toLightVector.xyz, lightID), lightDistance01);

	return lColor * lambert * falloff * cubeMapShadowSampleStat * cubeMapShadowSampleDynm;// * shadowFactor;
}

void main(){
	vec4 tex = texture(_Texture, uv + _UvOffset) * 0.5; // * 0.1
	color = _Diffuse * tex; // * vec4(pos.xyz / pos.w, 1);

	for (int i = 0; i < _LightCount; i++){
		if (_LightActives[i] == 1){
			color += tex * calculateLight(i) * 3;
		}
	}
}