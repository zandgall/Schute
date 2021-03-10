#shader vertex
#version 400 core

layout(location = 0) in vec4 position;
in vec2 uv;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 ortho;
uniform float layer;
uniform bool ui;

void main() {
	TexCoord = uv;
	vec4 pos = ortho * vec4((view* (model * position)).xy, 0.0, 1.0);
	if (ui)
		pos = (ortho*vec4((model*(position)).xy, 0, 0)) + vec4(-1, 1, 0, 0);
	gl_Position = pos;
	pos = ortho * vec4((view * (model * position)).xyz, 1.0);
	gl_Position.w = pos.w - (layer / 100.0);
	if (ui)
		gl_Position.w = 1;
}

#shader fragment
#version 400 core

in vec2 TexCoord;

layout(location = 0) out vec4 glColor;

uniform sampler2D diffuse;
uniform mat4 uvTransform;

uniform bool useCol;
uniform vec4 inCol;

uniform vec4 colMult;
uniform vec4 colAdd;

uniform bool alphaTest;
void main() {
	vec2 coord = (uvTransform * vec4(TexCoord, 0, 1)).xy;
	glColor = texture2D(diffuse, coord)*colMult + colAdd;
	if (useCol)
		glColor = inCol;

	if (alphaTest) {
		if (glColor.a < 0.8)
			discard;
		glColor.a = 1;
	}
	if (glColor.a == 0)
		discard;
}