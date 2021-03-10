#shader vertex
#version 400 core

layout(location = 0) in vec4 position;
in vec2 aTexCoord;

out vec3 pos;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	//gl_Position = projection * view * model * position;
	//gl_Position = pos * model;
	gl_Position = vec4(pos.xyz, 1.0);
	pos = vec3(model * position);
	TexCoord = aTexCoord;
}

#shader fragment
#version 400 core

struct Material {
	sampler2D normalMap;
	bool usenormalMap;
	sampler2D bumpMap;
	bool usebumpMap;
	sampler2D displacementMap;
	bool usedisplacementMap;
	sampler2D glossyMap; //
	bool useglossyMap; //
	sampler2D emmisionMap; //
	bool useemmisionMap; //

	vec3 ambient; //
	vec2 ambienceRange; //
	sampler2D ambientMap; //	
	vec3 diffuse; // 
	vec2 diffuseRange; // 
	sampler2D diffuseMap; //
	bool usediffuseMap; //
	vec3 specular; //
	vec2 specularRange; //
	sampler2D specularMap; // 
	bool usespecularMap; //

	float specularStrength; //
	float hardness; //
	float emmision; //
	float reflective;
	float refractive;
	float ior;
};

struct Light {
	vec3 position;
	vec4 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
	float cutoff;
	float outercutoff;
};

out vec4 glcolor;

in vec3 pos;
in vec2 TexCoord;

uniform Material material;
uniform vec4 color;
uniform vec3 viewPos;

uniform int lightCount;
uniform Light lights[32];

uniform samplerCube skybox;

float mapRange(in float num, in float inmin, in float inmax, in float outmin, in float outmax) {
	return (num - inmin) * ((outmax - outmin) / (inmax - inmin)) + outmin;
}

vec3 mapRange(in vec3 a, in float inmin, in float inmax, in float outmin, in float outmax) {
	return vec3(mapRange(a.x, inmin, inmax, outmin, outmax), mapRange(a.y, inmin, inmax, outmin, outmax), mapRange(a.z, inmin, inmax, outmin, outmax));
}

void main() {

	//raw diffuse
	vec4 obColor = vec4(material.diffuse, 1.0);
	obColor *= texture(material.diffuseMap, TexCoord);

	//return compilation
	//glcolor = (diffuse + ambient) * (obColor * reflectColor * refractColor) + specular;
	glcolor = obColor;
	glcolor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}