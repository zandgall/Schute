#pragma once
#ifndef PLAYER_H
#define PLAYER_H
#include "Entity.h"
class Player : public Entity {
public:
	glm::ivec2 spawn, flip = glm::ivec2(1);
	bool climbing;
	std::vector<unsigned int> tex;
	double movetime = 0.0;
	float climbRot = 0;
	Player();
	~Player();
	void move(std::vector<Entity*> *list, char checker(int, int), double timespeed = 0.1666);
	void tick(std::vector<Entity*> *list, double timespeed = 0.1666);
	void render();
private:
	unsigned int getFrame();
};
#endif