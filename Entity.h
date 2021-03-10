#pragma once
#ifndef ENTITY_H
#define ENTITY_H
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iostream>
class Entity {
public:
	glm::vec2 position, acceleration, velocity, size;
	glm::vec2 *pos, *acc, *vel;
	glm::vec2 _vel;
	glm::u8vec4 aabb = glm::u8vec4(0);
	float mass, strength, elasticity, hardness;
	bool solid = false;

	std::string UUID;
	Entity();
	~Entity();
	virtual void force(glm::vec2 newtons);

	virtual void tick(std::vector<Entity*> *list, double timespeed = 0.1666) = 0;
	virtual void collide(std::vector<Entity*> *list, char checker(int, int), double timespeed = 0.1666);
	virtual	void move(std::vector<Entity*> *list, char checker(int, int), double timespeed = 0.1666);
	virtual void render() = 0;
private:
	void genUUID() {
		UUID = std::string();
		std::string l = "0123456789ABCDEF";
		UUID = "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX";
		for (int i = 0; i < 8; i++) {
			UUID[i] = l[std::rand()%16];
		}
		for (int i = 0; i < 4; i++) {
			UUID[i+9] = l[std::rand() % 16];
		}
		UUID[14] = '4';
		for (int i = 1; i < 4; i++) {
			UUID[i+14] = l[std::rand() % 16];
		}
		UUID[19] = l[std::rand() % 4 + 7];
		for (int i = 1; i < 4; i++) {
			UUID[i+19] = l[std::rand() % 16];
		}
		for (int i = 0; i < 12; i++) {
			UUID[i+24] = l[std::rand() % 16];
		}
	}
};
#endif