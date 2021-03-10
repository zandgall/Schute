#pragma once

#include "Entity.h"
#include <string>
#include "Neutron/ValueAnimation.h"

class Button : public Entity {
public:
	bool pressed, toggle;
	std::string action;
	float offset = 0.f;

	Button();
	Button(int x, int y, int size, bool toggle, std::string action);
	~Button();

	void tick(std::vector<Entity*>* list, double timespeed = 0.1666);
	void move(std::vector<Entity*>* list, char checker(int, int), double timespeed = 0.1666) {}
	void render();
private:
	glm::vec2 spawn;
	ValueAnimation animation;
	double animT = 0.0;
	static unsigned int texs[8];
	static bool texLoaded;
};

