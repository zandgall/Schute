#include "Button.h"
#include "Physics.h"

#include "Neutron/Neutron.h"
#include "Neutron/NeutronPasses.h"
#include "Neutron/NeutronProcessor.h"
#include "Neutron/NeutronModes.h"
#include "Neutron/NeutronWorld.h"
#include "Neutron/NeutronModes.h"
#include "GameEnvelope.h"
#include <algorithm>

unsigned int Button::texs[8];
bool Button::texLoaded = false;

Button::Button() {
	pressed = false;
	spawn = glm::vec2(0);
	toggle = false;
}

Button::Button(int x, int y, int size, bool toggle, std::string action): Entity() {
	position = glm::vec2(x, y + 0.375);
	spawn = glm::vec2(x, y + 0.375);
	this->size = glm::vec2(size, 0.625);
	this->toggle = toggle;
	this->action = action;
	hardness = 0.5;
	std::vector<KeyFrame> f = std::vector<KeyFrame>();
	f.push_back(KeyFrame(0, 0, KeyFrame::CALCCUBIC, KeyFrame::TYPEOUT));
	f.push_back(KeyFrame(0.25, 1, 0, 0));
	animation = ValueAnimation(0, 1.0, f);
	solid = true;
	offset = -.125;

	if (!texLoaded) {
		unsigned int buttonTEX = neutron::processor::getTextureFrom("res/textures/button.png", GL_NEAREST, GL_CLAMP_TO_EDGE);
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 4; j++) {
				texs[i * 4 + j] = neutron::processor::getCroppedTexture(buttonTEX, j * 16, i * 8, 16, 8);

			}
		}
	}
}

Button::~Button() {}

void Button::tick(std::vector<Entity*>* list, double timespeed) {

	//std::vector<Entity*> s;

	//pressed = false;
	//for (Entity* e : *list) {
	//	if (e == this || !e->solid)
	//		continue;
	//	if (AABB(e->position, e->size, position, size)) {
	//		pressed = true;
	//		s.push_back(e);
	//	}
	//}

	glm::vec2 a = P::field.getForce(glm::vec4(position, size));
	
	a.y -= (size.x) * 7500.0;

	std::vector<std::string> sources = P::field.getForceSource(glm::vec4(position, size));

	//animT = std::min(std::max(animT + 4.0*(pressed ? timespeed : -timespeed), 0.0), 1.0);
	//animation.setTime(animT);
	if (!pressed || !toggle) {
		offset += (a.y / 100.f) * timespeed * timespeed;
		offset = std::min(std::max(offset, 0.f), 0.25f);
	}
	pressed = offset >= 0.25;
	GameEnvelope::events[action] = pressed;
	size.y = 0.625 - offset;
	float prY = position.y;
	position.y = spawn.y + offset;

	for (Entity* e : *list)
		for(std::string s : sources)
			if (s == e->UUID) {
				e->position.y += (position.y - prY);
				e->position.y = std::min(position.y - e->size.y, e->position.y);
			}
};

void Button::render() {
	using namespace neutron;
	if (size.x == 1) {
		passes::transform = processor::rect(position.x, position.y - 0.125, 1, 0.5);
		draw(texs[3]);
		passes::transform = processor::rect(position.x, position.y - offset + 0.125, 1, 0.5);
		draw(texs[7]);
	}
	else {
		for (int i = 0; i < size.x; i++) {
			if (i == 0) {
				passes::transform = processor::rect(position.x + i, position.y-0.125, 1, 0.5);
				draw(texs[0]);
				passes::transform = processor::rect(position.x + i, position.y-offset+0.125, 1, 0.5);
				draw(texs[4]);
			}
			else if (i == size.x - 1) {
				passes::transform = processor::rect(position.x + i, position.y - 0.125, 1, 0.5);
				draw(texs[2]);
				passes::transform = processor::rect(position.x + i, position.y - offset + 0.125, 1, 0.5);
				draw(texs[6]);
			}
			else {
				passes::transform = processor::rect(position.x + i, position.y - 0.125, 1, 0.5);
				draw(texs[1]);
				passes::transform = processor::rect(position.x + i, position.y - offset + 0.125, 1, 0.5);
				draw(texs[5]);
			}
		}
	}
};