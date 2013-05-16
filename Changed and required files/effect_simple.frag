#version 120

uniform sampler2D inSampler;
uniform sampler2D ColorCorrMap;
varying vec2 f_texcoord;

void main(void) {


    vec2 texcoord = f_texcoord;
	vec3 InColor = texture2D(inSampler, texcoord).xyz;
	vec3 OutColor;

	// Hocam burada bu fonksiyonu : "vec4 texture2D(sampler2D s, vec2 coord [, float bias])" kullandigimda goruntu cikti
	// aksi takdirda ekran bos cikiyordu
	/*
	OutColor.x = texture1D(ColorCorrMap, InColor.x).x;
	OutColor.y = texture1D(ColorCorrMap, InColor.y).y;
	OutColor.z = texture1D(ColorCorrMap, InColor.z).z;
	*/
	
	OutColor.x = texture2D(ColorCorrMap, vec2(InColor.x, 1)).x;
	OutColor.y = texture2D(ColorCorrMap, vec2(InColor.y, 1)).y;
	OutColor.z = texture2D(ColorCorrMap, vec2(InColor.z, 1)).z;
	
    gl_FragColor = vec4(OutColor, 1);
}