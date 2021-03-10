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
out vec4 FragColor;

in vec2 uv;

uniform sampler2D image;
uniform float bloomAmount;

uniform bool horizontal;
uniform float SPREAD = 5;

float weight(float x, float spread) {
	return 1/pow(2, ((x * x) / (2 * spread * spread)));
}

void main()
{
	vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
	vec3 result = texture(image, uv).rgb; // current fragment's contribution
	if (horizontal)	{
		for (int i = 1; i < SPREAD; ++i) {
			result += texture(image, uv + vec2(tex_offset.x * i, 0.0)).rgb * weight(i, SPREAD);
			result += texture(image, uv - vec2(tex_offset.x * i, 0.0)).rgb * weight(i, SPREAD);
		}
	}
	else {
		for (int i = 1; i < SPREAD; ++i) {
			result += texture(image, uv + vec2(0.0, tex_offset.y * i)).rgb * weight(i, SPREAD);
			result += texture(image, uv - vec2(0.0, tex_offset.y * i)).rgb * weight(i, SPREAD);
		}
	}
	FragColor = vec4(result * bloomAmount, 1.0);
}