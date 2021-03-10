#include "Quark.h"
#include <string>
#include <sstream>
#include <fstream>
#include "Neutron.h"
#include "NeutronProcessor.h"
#include "NeutronPasses.h"
#include "NeutronModes.h"
#include "NeutronWorld.h"
using namespace neutron;
Quark::Quark(glm::vec3 position, float layer, glm::vec3 size, glm::vec3 rotation) {
	this->position = position;
	this->layer = layer;
	this->size = size;
	this->rotation = rotation;
	UINTS = std::vector<unsigned int>();
	FLOATS = std::vector<float>();
	BOOLS = std::vector<bool>();
	FLOAT4S = std::vector<glm::vec4>();
	setUINT(SHADER, passes::SHADER_FLAT);
	setUINT(DRAW, modes::DRAW_FLAT);
	setVao(passes::VAO_FLAT, 6);
	setF4(COLOR, glm::vec4(1));
	setF4(MULT_COLOR, glm::vec4(1));
	setF4(ADD_COLOR, glm::vec4(0));
	setF(GLYPH_CLIP, 0.5f);
	setF(GLYPH_SOFT_EDGE, 0.1f);
	setM4(TEXTURE_TRANSFORM, processor::form(glm::vec4(0)));
	setB(ALPHA_TEST, false);
}

Quark::~Quark()
{
}

void Quark::copyOptionsFromNeutron() {
	// Set Unsigned ints
	setUINT(MULT_TEXTURE, passes::mult_texture);
	setUINT(ADD_TEXTURE, passes::add_texture);
	setUINT(DISTANCE_TEXTURE, passes::distance_texture);
	setUINT(VAO, passes::vao);
	setUINT(SHADER, passes::shader);
	setUINT(DRAW, modes::draw);
	// Set Float4s
	setF4(MULT_COLOR, passes::mult_color);
	setF4(ADD_COLOR, passes::add_color);
	// Set Floats
	setF(GLYPH_CLIP, passes::glyph_clip);
	setF(GLYPH_SOFT_EDGE, passes::glyph_soft_edge);
	// Set Bools
	setB(GUI, modes::gui);
	setB(ALPHA_TEST, modes::alpha_test);
}

void Quark::tick() {
}

void Quark::set() {
	using namespace neutron::passes;
	mult_texture = getUINT(MULT_TEXTURE);
	add_texture = getUINT(ADD_TEXTURE);
	distance_texture = getUINT(DISTANCE_TEXTURE);
	vao = getUINT(VAO);
	shader = getUINT(SHADER);
	neutron::modes::draw = getUINT(DRAW);

	mult_color = getF4(MULT_COLOR);
	add_color = getF4(ADD_COLOR);

	glyph_clip = getF(GLYPH_CLIP);
	glyph_soft_edge = getF(GLYPH_SOFT_EDGE);

	neutron::modes::gui = getB(GUI);
	neutron::modes::alpha_test = getB(ALPHA_TEST);
	neutron::passes::rotation = this->rotation;
	face_camera = getB(FACE_CAMERA);
	transform = neutron::processor::form(position, size, this->rotation);
	uvTransform = getM4(TEXTURE_TRANSFORM);
}

void Quark::render() {
	set();
	draw(getUINT(TEXTURE), getF4(COLOR), layer, vertCount);
	set();
	drawText(getS(Quark::TEXT), getS(Quark::FONT).c_str(), getUINT(TEXTURE), getF4(COLOR), layer, vertCount);
}

int sti(std::string in) {
	if (in == "")
		return 1;
	else return std::stoi(in);
}

void printFa(float a[], int size = 9) {
	std::cout << "{";
	std::cout << "Size: " << size << ", ";
	for (int i = 0; i < size-1; i++) {
		std::cout << *(a+i) << ", ";
	}
	std::cout << a[size - 1] << "}" << std::endl;
}

void Quark::loadOBJ(std::string filepath) {
	std::vector<glm::vec4> positions = std::vector<glm::vec4>();
	std::vector<glm::vec3> normal = std::vector<glm::vec3>();
	std::vector<glm::vec2> uv = std::vector<glm::vec2>();
	std::vector<glm::mat3> faces = std::vector<glm::mat3>();

	std::ifstream stream(filepath);

	std::string line;
	std::stringstream ss[2];

	std::vector<std::vector<float>> data = std::vector<std::vector<float>>();

	std::cout << "Loading object     " << filepath << std::endl;

	while (std::getline(stream, line)) {
		if (line.find("vn ") != std::string::npos) {
			std::vector<std::string> values = neutron::processor::split(line, ' ');
			normal.push_back(glm::vec3(std::stod(values[1]), std::stod(values[2]), std::stod(values[3])));
		}
		else if (line.find("vt ") != std::string::npos) {
			std::vector<std::string> values = neutron::processor::split(line, ' ');
			uv.push_back(glm::vec2(std::stod(values[1]), std::stod(values[2])));
		}
		else if (line.find("v ") != std::string::npos) {
			std::vector<std::string> values = neutron::processor::split(line, ' ');
			positions.push_back(glm::vec4(std::stod(values[1]), std::stod(values[2]), std::stod(values[3]), 1));
		}
		else if (line.find("f ") != std::string::npos) {
			std::vector<std::string> vertices = neutron::processor::split(line, ' ');
			std::vector<std::string> v1 = neutron::processor::split(vertices[1], '/');
			std::vector<std::string> v2 = neutron::processor::split(vertices[2], '/');
			std::vector<std::string> v3 = neutron::processor::split(vertices[3], '/');
			faces.push_back(glm::mat3(
				glm::vec3(sti(v1[0]), sti(v1[1]), sti(v1[2])), 
				glm::vec3(sti(v2[0]), sti(v2[1]), sti(v2[2])), 
				glm::vec3(sti(v3[0]), sti(v3[1]), sti(v3[2]))
			));
		}
	}

	std::vector<unsigned int> ind = std::vector<unsigned int>();
	std::cout << "Optimizing indices " << filepath << std::endl;

	for (int i = 0; i < faces.size(); i++)
		for (short j = 0; j < 3; j++) {
			glm::vec4 pos = positions[faces[i][j].x - 1];
			glm::vec3 norm = glm::vec3(0, 0, -1);
			if (faces[i][j].y <= normal.size()) norm = normal[faces[i][j].y - 1];
			glm::vec2 tex = glm::vec2(0);
			if (faces[i][j].z <= uv.size()) tex = uv[faces[i][j].z - 1];
			std::vector<float> con = { pos.x, pos.y, pos.z, pos.w, norm.x, norm.y, norm.z, tex.x, tex.y };

			int found = -1;
			for (int h = 0; h < data.size(); h++) {
				if (data[h] == con) {
					found = h;
					break;
				}
			}
			if (found == -1) {
				ind.push_back(data.size());
				data.push_back(con);
			}
			else
				ind.push_back(found);
		}

	float *pos = new float[data.size()*9];
	for (int i = 0; i < data.size(); i++)
		for (int j = 0; j < data[i].size(); j++)
			pos[i * 9 + j] = data[i][j];
	unsigned int vao = neutron::processor::VAO();
	unsigned int vbo = neutron::processor::VBO();
	unsigned int ebo = neutron::processor::EBO();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size()*9*sizeof(float), pos, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size()*sizeof(unsigned int), &ind[0], GL_STATIC_DRAW);

	std::cout << ind.size() << std::endl;

	processor::attribute(0, 4, 9, 0);
	processor::attribute(1, 2, 9, 7);
	processor::attribute(2, 3, 9, 4);

	setVao(vao, ind.size());
}

bool Quark::sort::operator()(Quark* a, Quark* b) const {
	glm::vec3 camPos = world::camera.position;
	if (world::camera.moveMode == world::Camera::MODE_ORBIT)
		camPos = world::camera.position - world::camera.Front*world::camera.OrbitDistance;
	glm::vec4 ap = passes::globalTransform * glm::vec4(a->position, 1);
	glm::vec4 bp = passes::globalTransform * glm::vec4(b->position, 1);
	return glm::distance(glm::vec3(ap.x, ap.y, ap.z), camPos) - a->layer / 100.0f > glm::distance(glm::vec3(bp.x, bp.y, bp.z), camPos) - b->layer / 100.0f;
}