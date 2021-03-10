#shader vertex
#version 400 core

layout(location = 0) in vec4 position;
in vec2 uv;
in vec3 _normal;
layout(location = 3) in mat4 instance;

in int gl_InstanceID;

out vec2 TexCoord;
out vec3 normal;
out vec4 zoom;
out vec3 objpos;

uniform mat4 parent;
uniform mat4 view;
uniform mat4 ortho;
uniform float layer;
uniform bool ui;

void main() {
	TexCoord = uv;
	mat4 model = parent*instance;
	vec4 pos = ortho * vec4((view* (model * position)).xy, 0.0, 1.0);
	if (ui)
		pos = (ortho*vec4((model * (position)).xy, 0, 0)) + vec4(-1, 1, 0, 0);
	gl_Position = pos;
	pos = ortho * vec4((view * (model * position)).xyz, 1.0);
	gl_Position.w = pos.w - (layer / 100.0);
	if (ui)
		gl_Position.w = 1;
	gl_Position / 2;
	normal = _normal;
	zoom = (ortho*view)[0];
	objpos = (view* (model * position)).xyz;
}

#shader fragment
#version 400 core

in vec2 TexCoord;
in vec3 normal;
in vec4 zoom;
in vec3 objpos;

layout(location = 0) out vec4 glColor;

uniform sampler2D diffuse;
uniform mat4 uvTransform;

uniform bool useCol;
uniform vec4 inCol;

uniform bool useMask;
uniform sampler2D mask;

uniform bool isGlyph;
uniform sampler2D distance;
uniform bool useNearestMask;
uniform sampler2D nearestMask;
uniform float glyphClip;
uniform float softEdge;

uniform vec4 colMult;
uniform vec4 colAdd;
uniform bool useAddTexture;
uniform sampler2D addTex;
uniform bool useMultTexture;
uniform sampler2D multTex;

uniform bool alphaTest;

vec4 cubic(float v) {
	vec4 n = vec4(1, 2, 3, 4) - v;
	vec4 s = n * n * n;
	float x = s.x;
	float y = s.y - 4 * s.x;
	float z = s.z - 4.0 * s.y + 6.0 * s.x;
	float w = 6.0 - x - y - z;
	return vec4(x, y, z, w) * (1.0 / 6.0);
}

vec4 textureBi(sampler2D sampler, vec2 coords) {
	vec2 texSize = textureSize(sampler, 0);
	vec2 invTexSize = 1.0 / texSize;

	coords = coords * texSize - 0.5;

	vec2 fxy = fract(coords);
	coords -= fxy;

	vec4 xcubic = cubic(fxy.x);
	vec4 ycubic = cubic(fxy.y);

	vec4 c = coords.xxyy + vec2(-0.5, 1.5).xyxy;

	vec4 s = vec4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
	vec4 offset = c + vec4(xcubic.yw, ycubic.yw) / s;

	offset *= invTexSize.xxyy;

	vec4 sample0 = texture(sampler, offset.xz);
	vec4 sample1 = texture(sampler, offset.yz);
	vec4 sample2 = texture(sampler, offset.xw);
	vec4 sample3 = texture(sampler, offset.yw);

	float sx = s.x / (s.x + s.y);
	float sy = s.z / (s.z + s.w);

	return mix(
		mix(sample3, sample2, sx), mix(sample1, sample0, sx)
		, sy);
}

//http://casual-effects.com/research/Wyman2017Improved/index.html
float hash(vec2 coord) {
	return fract(1.0e4 * sin(17.0*coord.x + 0.1*coord.y) *
		(0.1 + abs(sin(13.0*coord.y + coord.x)))
	);
}
float hash3D(vec3 coord) {
	return hash(vec2(hash(coord.xy), coord.z));
}

void main() {

	vec2 coord = (vec4(TexCoord, 0, 1) * uvTransform).xy;
	glColor = texture2D(diffuse, coord)*colMult + colAdd;
	if (useCol)
		glColor = inCol;
	if (useMask)
		glColor.w = texture2D(mask, coord).r;

	if (isGlyph) {
		float s = max((1 - zoom.x + 10) / 1000, 0);
		float alpha = textureBi(distance, coord).r;
		if (useNearestMask && texture2D(nearestMask, coord).r == 0)
			discard;
		if (useNearestMask && texture2D(nearestMask, coord).r == 1)
			alpha = 10;
		alpha = smoothstep(glyphClip - softEdge, glyphClip + softEdge, alpha);
		glColor.w *= alpha;
	}

	if (useAddTexture)
		glColor += texture2D(addTex, coord);
	if (useMultTexture)
		glColor *= texture2D(multTex, coord);

	if (alphaTest) {
		if (glColor.a < 0.8)
			discard;
		glColor.a = 1;
	}
}