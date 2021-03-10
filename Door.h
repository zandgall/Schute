#pragma once
#include "Entity.h"
class Door : public Entity {
public:
	bool toggle, horizontal;
	std::string action;
	float length;
	int maxLength;
	Door();
	Door(int x, int y, int size, bool toggle, bool horizontal, std::string action);
	~Door();
	void tick(std::vector<Entity*>* list, double timespeed = 0.1666);
	void move(std::vector<Entity*>* list, char checker(int, int), double timespeed = 0.1666) {}
	void render();
private:
	static int door[];
	bool toggledOn = false;
};

