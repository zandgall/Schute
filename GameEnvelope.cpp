#include "Neutron/NeutronProcessor.h"
#include "GameEnvelope.h"
#include "World.h"
#include "Neutron/Neutron.h"
#include "Neutron/NeutronPasses.h"
#include "Neutron/NeutronModes.h"
#include "Neutron/NeutronFrame.h"
#include "Neutron/Quark.h"
#include "FileIO/FileChooser.h"
#include "FileIO/CompoundTag.h"
#include "Rock.h"
#include "Neutron/stb_image.h"
#include "Player.h"
#include "Vine.h"
#include "Door.h"
#include "Button.h"
#include "Physics.h"
#include <iostream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
using namespace neutron;
World w;

std::map<std::string, bool> GameEnvelope::events = std::map<std::string, bool>();

// Source: https://stackoverflow.com/a/46711735
// Copied from: https://gcc.godbolt.org/z/pfJXRm
constexpr inline long long int hash(char const* str, int h = 0) {
	return (!str[h] ? 5381 : (hash(str, h + 1) * 33) ^ str[h]);
}

constexpr inline long long int operator "" _(char const* p, size_t) { return hash(p); }

inline long long int hash(std::string const& s) { return hash(s.c_str()); }
// End of string 'hashing'

char g(int x, int y) {
	return w.get(x, y);
}

void GameEnvelope::load(std::string path) {
	P::field.force.clear();
	P::field.force_.clear();
	P::field.force_source.clear();
	CompoundTag t = loadNBT(path.c_str());
	CURR_PATH = path;
	w.set(t.getIntTag("width")->getValue(), t.getIntTag("height")->getValue());
	std::vector<unsigned char> tiles = t.getByteArrayTag("tiles")->getValue();
	for (auto i = 0; i < tiles.size(); i++)
		w.tiles[i] = tiles[i];
	tiles.clear();

	int xx, yy, com;
	std::vector<unsigned char> imgArray = t.getByteArrayTag("baseImg")->getValue();
	stbi_set_flip_vertically_on_load(true);
	unsigned char* img = stbi_load_from_memory(&imgArray[0], imgArray.size(), &xx, &yy, &com, 4);
	glBindTexture(GL_TEXTURE_2D, w.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xx, yy, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	stbi_image_free(img);
	
	entities = std::vector<Entity *>();

	ListTag ents = t.li("entities");
	auto e = ents.getValue();
	for (auto i = e.begin(); i != e.end(); i++) {
		CompoundTag* t = dynamic_cast<CompoundTag*>(*i);
		int x = t->i("x")();
		int y = t->i("y")();
		switch (hash(t->S("type")())) {
		case "rock"_:
			entities.push_back(new Rock(x, y));
			break;
		case "coin"_:
			break;
		case "button"_:
			entities.push_back(new Button(x, y, t->i("size")(), t->b("toggle")(), t->S("event")()));
			break;
		case "door"_:
			entities.push_back(new Door(x, y, t->i("size")(), t->b("toggle")(), t->b("horizontal")(), t->S("event")()));
			break;
		}
	}

	Player* p = new Player();
	center = p;
	p->position.x = t.getIntTag("spawnX")->getValue();
	p->position.y = t.getIntTag("spawnY")->getValue();
	p->spawn.x = t.getIntTag("spawnX")->getValue();
	p->spawn.y = t.getIntTag("spawnY")->getValue();
	entities.push_back(p);

	ListTag bg = t.li("bg");
	
	for (int i = 0; i < bg.getValue().size(); i++) {
		CompoundTag *j = bg.get<CompoundTag>(i);

		imgArray = j->getByteArrayTag("img")->getValue();

		unsigned int bgTex = 0;
		img = stbi_load_from_memory(&imgArray[0], imgArray.size(), &xx, &yy, &com, 4);
		glGenTextures(1, &bgTex);
		glBindTexture(GL_TEXTURE_2D, bgTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xx, yy, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(img);

		bgelements.insert(std::make_pair(j->getFloatTag("layer")->getValue(), bgTex));
	}
	passes::SHADER_FLAT = processor::LoadShader("res/shaders/schutedef.shader", true);
	frame::delta_time = 0;
}

GameEnvelope::GameEnvelope() {
	P::field = Field<glm::vec4, glm::vec2>();
	w = World();
	load(ChooseOpenFile("Pick world", "Binary World Files (*.bin)\0*.bin\0All Files (*.*)\0*.*"));
}


GameEnvelope::~GameEnvelope() {

}

void GameEnvelope::tick() {
	float incr = pow(10, frame::keys::keys[GLFW_KEY_LEFT_CONTROL] - frame::keys::keys[GLFW_KEY_LEFT_SHIFT]);
	float r = min(frame::delta_time*10, 1);
	scroll += frame::delta_time * frame::mouse::y_s_g * 10 * incr;
	xOff = xOff * (1 - r) + (center->pos->x - (frame::width / scroll)/2) * scroll * r;
	yOff = yOff * (1 - r) + (center->pos->y - (frame::height/ scroll)/2) * scroll * r;
	xOff = (std::max)((std::min)(xOff, w.width*scroll - frame::width), 0.0);
	yOff = (std::max)((std::min)(yOff, w.height*scroll - frame::height), 0.0);
	for (int i = 0, s = entities.size(); i < s; i++) {
		entities[i]->tick(&entities, frame::delta_time);
		entities[i]->move(&entities, g, frame::delta_time);
	}
	if (frame::keys::keys[GLFW_KEY_LEFT_CONTROL] && frame::keys::keys[GLFW_KEY_R])
		load(CURR_PATH);
}

void GameEnvelope::render() {

}

void GameEnvelope::render_gui() {
	modes::gui = true;
	modes::alpha_test = false;
	passes::shader = processor::LoadShader("res/shaders/schutedef.shader", true);
	for (std::map<float, unsigned int>::iterator i = bgelements.begin(); i != bgelements.end(); i++) {
		passes::globalTransform = processor::form(glm::vec3(-(float)xOff * (i->first + 1.f), -(float)yOff * (i->first + 1.f), 0.f), glm::vec3(scroll));
		passes::transform = glm::translate(glm::scale(processor::rect(0, 0, w.width, w.height), glm::vec3(1, -1, 1)), glm::vec3(0, 0, 0));
		draw(i->second);
	}

	passes::globalTransform = processor::form(glm::vec3(-xOff, -yOff, 0), glm::vec3(scroll));
	passes::transform = glm::scale(processor::rect(0, 0, w.width, w.height), glm::vec3(1, -1, 1));
	draw(w.texture);
	for (int i = 0, s = entities.size(); i < s; i++) {
		entities[i]->render();
	}

	passes::shader = processor::LoadShader("res/shaders/schutedef.shader", true);
	for (int s = 0; s < P::field.force.size(); s++) {
		passes::transform = processor::rect(P::field.force_[s].x, P::field.force_[s].y, P::field.force_[s].z, P::field.force_[s].w);
		float m = glm::distance(glm::vec2(0), P::field.force[s])/1000.0;
		draw(0, glm::vec4(m, 2 - m, 0, 1));
		//draw(0, glm::vec4(1, 0, 0, 1));
	}
}