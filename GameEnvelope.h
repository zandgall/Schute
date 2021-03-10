#pragma once
#ifndef GAMEENVELOPE_H
#define GAMEENVELOPE_H
#include "Envelope.h"
#include "Entity.h"
#include <vector>
#include <string>
#include <map>
class GameEnvelope: public Envelope {
public:

	double xOff = 0, yOff = 0, xOffVel = 0, yOffVel = 0, scroll = 70.0;

	std::map<float, unsigned int> bgelements = std::map<float, unsigned int>();

	static std::map<std::string, bool> events;

	std::vector<Entity*> entities;

	Entity* center;

	std::string CURR_PATH;

	GameEnvelope();
	~GameEnvelope();
	void tick();
	void render();
	void render_gui();
	void load(std::string path);
};
#endif