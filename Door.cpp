#include "Door.h"
#include "GameEnvelope.h"
#include "Neutron/Neutron.h"
#include "Neutron/NeutronProcessor.h"
#include "Neutron/NeutronPasses.h"
using namespace neutron;

int Door::door[] = {0, 0, 0, 0, 0};

Door::Door() {
	toggle = false;
	length = 4;
	maxLength = 4;
	action = "event";
	horizontal = false;
	this->size = glm::vec2(1, length);
	this->solid = true;
}

Door::Door(int x, int y, int size, bool toggle, bool horizontal, std::string action) {
	this->action = action;
	this->toggle = toggle;
	this->horizontal = horizontal;
	position = glm::vec2(x, y);
	length = size;
	maxLength = size;
	this->solid = true;
	this->size = glm::vec2((horizontal ? size : 1), (horizontal ? 1 : size));
	if (door[0] == 0) {
		int tex = processor::getTextureFrom("res/textures/door.png", GL_NEAREST, GL_CLAMP_TO_EDGE);
		door[0] = processor::getCroppedTexture(tex, 0, 0, 16, 16);
		door[1] = processor::getCroppedTexture(tex, 0, 16, 16, 16);
		door[2] = processor::getCroppedTexture(tex, 16, 0, 16, 16);
		door[3] = processor::getCroppedTexture(tex, 16, 16, 16, 16);
		door[4] = processor::getCroppedTexture(tex, 16, 32, 16, 16);
	}
}

Door::~Door() {

}

void Door::tick(std::vector<Entity*>* list, double timespeed) {
	toggledOn = GameEnvelope::events[action] && toggle;
	if (GameEnvelope::events[action] || toggledOn)
		length -= timespeed;
	else length += timespeed;

	if (length > maxLength)
		length = maxLength;
	else if (length < 0)
		length = 0;

	this->size = glm::vec2(horizontal ? length : 1, horizontal ? 1 : length);
}

void Door::render() {
	if (horizontal) {
		passes::transform = processor::rect(pos->x, pos->y, length, 1);
		passes::uvTransform = processor::rect(-1 - length, -1-length, 2 * length, 2, glm::vec3(0, 0, 3.14159265 / 2.0));
		neutron::draw((GameEnvelope::events[action] || toggledOn) ? (fmod(glfwGetTime(), 1.0) < 0.5 ? door[3] : door[4]) : door[2]);
		passes::uvTransform = processor::rect(-1, -1, 2, 2, glm::vec3(0, 0, 3.14159265 / 2.0));
		passes::transform = processor::rect(pos->x, pos->y, 1, 1);
		neutron::draw((GameEnvelope::events[action] || toggledOn) ? door[1] : door[0]);
		passes::uvTransform = processor::rect(-1, -1, 2, 2);
	}
	else {
		passes::transform = processor::rect(pos->x, pos->y, 1, length);
		passes::uvTransform = processor::rect(-1, -1, 2, 2 * length);
		neutron::draw((GameEnvelope::events[action] || toggledOn) ? (fmod(glfwGetTime(), 1.0) < 0.5 ? door[3] : door[4]) : door[2]);
		passes::uvTransform = processor::rect(-1, -1, 2, 2);
		passes::transform = processor::rect(pos->x, pos->y, 1, 1);
		neutron::draw((GameEnvelope::events[action] || toggledOn) ? door[1] : door[0]);
	}
}