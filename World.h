#pragma once
#ifndef WORLD_H
#define WORLD_H
#include <string>
#include <vector>
#include <windows.h>
class World {
public:
	static unsigned short tileSize;
	int width = 0, height = 0;
	unsigned int texture = 0;
	std::vector<unsigned char> tiles;
	World();
	~World();
	void set(int width, int height);
	unsigned char get(int x, int y);
	void updateTexture();
private:
	unsigned int framebuffer = 0;
};
#endif