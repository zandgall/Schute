#pragma once
#ifndef SERIALIMAGE_H
#define SERIALIMAGE_H
#include <vector>
#include <glm/glm.hpp>
class SerialImage
{
public:
	int width, height, nrChannels;
	std::vector<glm::vec4> colors;
	SerialImage(const char* filepath, bool vertflip = true);
	unsigned int texture;

	SerialImage() {
		colors = std::vector<glm::vec4>();
		width = 0;
		height = 0;
		nrChannels = 4;
		texture = 0;
	}
	SerialImage(std::vector<glm::vec4> colors, int width, int height, int nrChannels = 4);
	~SerialImage();
	SerialImage getCropped(int x, int y, int width, int height);
	void crop(int x, int y, int width, int height);
};
#endif