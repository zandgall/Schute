#pragma once
#include "Entity.h"
class Rock :
	public Entity
{
public:
	Rock();
	Rock(int x, int y): Entity(){
		size = glm::vec2(3);
		solid = true;
		this->pos->x = x;
		this->pos->y = y;
		this->mass = 125;
		this->hardness = 10;
	}
	~Rock();
	
	void force(glm::vec2 newtons) {
		if (aabb.w == 0)
			acceleration = glm::vec2(0);
		else Entity::force(newtons);
	}

	void tick(std::vector<Entity*> *list, double timespeed = 0.1666) {

	}

	void render();
};

