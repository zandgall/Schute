#shader vertex
#version 400 core

layout(location = 0) in vec4 position;

out vec2 coord;

uniform vec4 crop;

void main() {
	vec2 c = (position.zw*crop.zw)+crop.xy;
	coord = vec2(c.x, 1 - c.y);

	gl_Position = vec4(position.xy, 1.0, 1.0);
}

#shader fragment
#version 400 core

in vec2 coord;

layout(location = 0) out vec4 glColor;

uniform sampler2D tex;

void main() {
	glColor = texture2D(tex, coord);
}