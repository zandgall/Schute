#pragma once
#ifndef NEUTRON_H
#define NEUTRON_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include<iostream>
#include <vector>
#include <map>

#include <glm/glm.hpp>

extern GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

namespace neutron {

	// Use this before your main loop
	extern void beforeLoop();
	// Use this after your main loop
	extern void afterLoop();

	// Sets every value to it's given default, useful for customizing your default gamemode/drawmode : like a soft reset
	extern void reset();

	// Sets every default value, required on startup : like a hard reset
	extern void set(std::string name = "Neutron", int width = 800, int height = 800);

	extern bool isSet();

	extern int setUniforms(unsigned int texture = 0, glm::vec4 color = glm::vec4(1), float layer = 0.f, unsigned int vertices = 0);

	extern void closeTextures(unsigned int number = 32);

	extern void draw(unsigned int texture = 0, glm::vec4 color = glm::vec4(1), float layer = 0.f, unsigned int vertices = 0);

	extern void drawInstances(std::vector<glm::mat4> instances, unsigned int texture = 0, glm::vec4 color = glm::vec4(1), float layer = 0.f, unsigned int vertices = 0);

	extern void drawText(const std::string &text, const char* font, unsigned int texture, glm::vec4 color, float layer, unsigned int vertices, float xspacing = 0, float yspacing = 0);
	extern void drawText(const std::u32string &text, const char* font, unsigned int texture, glm::vec4 color, float layer, unsigned int vertices, float xspacing = 0, float yspacing = 0);
};
#endif
