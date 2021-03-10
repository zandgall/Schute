#shader vertex
#version 400 core

layout(location = 1) in vec4 position;
in vec2 _uv;
in vec3 normal;

out vec2 uv;

void main() {
	mat4 trans = mat4(vec4(2, 0, 0, 0), vec4(0, 2, 0, 0), vec4(0, 0, 1, 0), vec4(0, 0, 0, 1));
	gl_Position = (trans*position - vec4(1, 1, 0, 0));
	uv = position.xy;
}

#shader fragment
in vec2 uv;

out vec4 glColor;

uniform sampler2D image;
uniform float clipAt = 0.9, softEdge = 0.1;

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

void main() {
	vec3 hsvC = rgb2hsv(texture2D(image, uv).xyz);
	glColor = texture2D(image, uv) * smoothstep(clipAt - softEdge, clipAt + softEdge, hsvC.z);
	glColor.w = 1;
}