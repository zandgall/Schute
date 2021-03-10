#shader vertex
#version 400 core

layout(location = 0) in vec4 position;
in vec2 uv;
in vec3 _normal;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 ortho;
uniform mat4 rotate;
uniform float layer;
uniform bool ui;
uniform vec4 dir;
void main() {
	TexCoord = uv;
	vec4 pos = (rotate * (position + dir)) - dir;
	pos = (ortho*vec4((model*(pos)).xy, 0, 0)) + vec4(-1, 1, 0, 0);
	gl_Position = pos;
	gl_Position.w = 1;
}

#shader fragment
#version 400 core

in vec2 TexCoord;

layout(location = 0) out vec4 glColor;

uniform sampler2D diffuse;

uniform mat4 uvTransform;
uniform vec4 colMult;
uniform vec4 colAdd;

void main() {
	glColor = texture2D(diffuse, (uvTransform*vec4(TexCoord, 0, 1)).xy)*colMult+colAdd;
	//glColor = vec4(1);
	if (glColor.a == 0)
		discard;
}