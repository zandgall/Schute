#shader vertex
#version 400 core

layout(location = 1) in vec4 position;
in vec2 _uv;
in vec3 normal;

out vec2 uv;

void main() {
	uv = position.xy;
	mat4 trans = mat4(vec4(2, 0, 0, 0), vec4(0, 2, 0, 0), vec4(0, 0, 1, 0), vec4(0, 0, 0, 1));
	gl_Position = (trans*position - vec4(1, 1, 0, 0));
}

#shader fragment
in vec2 uv;

out vec4 glColor;

uniform sampler2D screen;

uniform float vignette;
uniform vec4 vignetteColor;

uniform sampler2D bloom;

uniform vec4 hsvMult, hsvAdd;
uniform vec4 rgbMult, rgbAdd;
uniform float gamma;

// THANK YOU http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
vec3 rgb2hsv(vec3 c) {
	vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
	vec4 p = c.g < c.b ? vec4(c.bg, K.wz) : vec4(c.gb, K.xy);
	vec4 q = c.r < p.x ? vec4(p.xyw, c.r) : vec4(c.r, p.yzx);

	float d = q.x - min(q.w, q.y);
	float e = 1.0e-10;
	return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
vec3 hsv2rgb(vec3 c) {
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec4 antialias(vec2 coord, vec2 size, sampler2D image) {
	vec4 c = texture2D(image, coord);
	vec4 sx0 = texture2D(image, coord + vec2(-1,  0) / size);
	vec4 sx1 = texture2D(image, coord + vec2( 1,  0) / size);
	vec4 sy0 = texture2D(image, coord + vec2( 0, -1) / size);
	vec4 sy1 = texture2D(image, coord + vec2( 0,  1) / size);
	return (c + sx0 + sx1 + sy0 + sy1) / 5;
}

void main() {
	
	vec2 size = textureSize(screen, 0);

	glColor = texture2D(screen, uv);
	
	// Bloom
	glColor += texture2D(bloom, uv);

	// Vignette
	float vDist = distance(uv, vec2(0.5, 0.5));
	vDist = smoothstep(0.5 - vignette, 0.5 + vignette, vDist);
	glColor = mix(glColor, glColor*vignetteColor, vDist);
	
	// HSV + RGB color correction and gamma
	vec3 hC = rgb2hsv(glColor.rgb);
	hC.z = pow(hC.z, 1 / gamma);
	hC += hsvAdd.rgb;
	hC *= hsvMult.rgb;
	glColor.xyz = hsv2rgb(hC);
	glColor += rgbAdd;
	glColor *= rgbMult;

	glColor = texture2D(screen, uv);
	//glColor.xy = uv;
}