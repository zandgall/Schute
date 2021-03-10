#shader vertex
#version 400
in vec4 position;
in vec2 auv;

out vec2 uv;

void main() {
	gl_Position = vec4(position);
	uv = auv;
}

#shader fragment
#version 400

in vec2 uv;

out vec4 color;

uniform sampler2D tex;

void main() {
	color = texture(tex, uv);
}