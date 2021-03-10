#pragma once
#include "Entity.h"
class Vine : public Entity {
public:
	bool growing = true, vertical = true, branch = false;
	double time = 0, rotation = 0, growth = 1;
	glm::vec2 dir = glm::vec2(0, -1), start = glm::vec2(0);
	Vine();
	~Vine();
	void tick(std::vector<Entity*> *list, double timespeed = 0.1666);
	void render();
	void move(std::vector<Entity*> *list, char checker(int, int), double timespeed = 0.1666);
};

