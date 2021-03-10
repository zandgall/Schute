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
#version 400 core

in vec2 uv;

out vec4 fragColor;

uniform mat4x3 corrections[32];
uniform sampler2D image;
uniform bool hsv;
uniform int length;
uniform float feather = 0.2;

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

float weight(float x, float spread) {
	if (spread == 0)
		return 0;
	return 1 / pow(2, ((x * x) / (2 * spread * spread)));
}
float fix(float a) { return max(min(a, 1), 0); }
float map(float a, float b, float c) {
	return fix((c - a)/(b-a));
}

void main() {
	if (hsv) {
		vec4 color = texture2D(image, uv);
		vec3 c = rgb2hsv(color.xyz);
		for (int i = 0; i < 32 && i < length; i++) {
			// Get how similar they are to the select hue based on a gussian curve with the select hue tolerance
			mat4x3 l = corrections[i];

			float hR = abs(l[0].x - c.x);
			hR = min(hR, 1-abs((l[0].x) - c.x));
			hR = smoothstep(l[1].x-feather, l[1].x+feather, hR);
			float sR = smoothstep(l[1].y - feather, l[1].y + feather, abs(l[0].y - c.y));
			float vR = smoothstep(l[1].z - feather, l[1].z + feather, abs(l[0].z - c.z));
			hR = fix(hR);
			sR = fix(sR);
			vR = fix(vR);

			// Get full ratio
			float r = max(hR, max(sR, vR));

			// Mix between original hue and multiplied and added hue
			c.y = r * c.y + (1 - r) * (c.y * l[2].y + l[3].y);
			c.z = r * c.z + (1 - r) * (c.z * l[2].z + l[3].z);
			r = step(0.001, r);
			c.x = r * c.x + (1 - r) * (c.x * l[2].x + l[3].x);
			
			fragColor = vec4(1-hR, 1-sR, 1-vR, color.w);
		}
		fragColor = vec4(hsv2rgb(c), color.w);
	}
	else {
		vec4 color = texture2D(image, uv);
		vec3 c = color.xyz;
		for (int i = 0; i < 32 && i<length; i++) {
			// Get how similar they are to the select hue based on a gussian curve with the select hue tolerance
			mat4x3 l = corrections[i];
			float hR = smoothstep(l[1].x - feather, l[i].x + feather, abs(l[0].x - c.x));
			float sR = smoothstep(l[1].y - feather, l[1].y + feather, abs(l[0].y - c.y));
			float vR = smoothstep(l[1].z - feather, l[1].z + feather, abs(l[0].z - c.z));

			// Get full ratio
			float r = max(hR, max(sR, vR));

			// Mix between original rgb and multiplied and added rgb
			c.x = r * c.x + (1 - r) * (c.x * l[2].x + l[3].x);
			c.y = r * c.y + (1 - r) * (c.y * l[2].y + l[3].y);
			c.z = r * c.z + (1 - r) * (c.z * l[2].z + l[3].z);
			fragColor = vec4(1 - hR, 1 - sR, 1 - vR, color.w);
		}
		fragColor = vec4(c, color.w);
	}
}