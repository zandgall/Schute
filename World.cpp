#include "World.h"
#include "Neutron/NeutronProcessor.h"
#include "Neutron/NeutronPasses.h"
#include "Neutron/SerialImage.h"
#include "Neutron/NeutronFrame.h"
#include "Neutron/Neutron.h"
#include <glm/gtc/matrix_transform.hpp>

unsigned short World::tileSize = 16;
World::World() {
	if (!neutron::isSet())
		return;
	width = 10;
	height = 5;
	tiles = std::vector<unsigned char>(width*height);
	for (int i = 0; i < width*height; i++)
		tiles[i] = false;

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &texture);				// gen texture,
	glBindTexture(GL_TEXTURE_2D, texture);  // and then bind it

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width*tileSize, height*tileSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);	// Set texture to empty as 2d, with RGB and A channels, using unsigned bytes


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);		// Use set interpolation close up
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);		// ..and far away
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Don't overlap horizontally
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// ..or vertically

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);	// Set a Color attachment to the frame buffer
	glDrawBuffer(GL_COLOR_ATTACHMENT0);										// Set a Color attachment to the draw buffer

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


World::~World() {

}

void World::set(int width, int height) {
	this->width = width;
	this->height = height;
	tiles.resize(width*height, 0x00);
}

unsigned char World::get(int x, int y) {
	if (x < 0 || y < 0 || x >= width || y >= height)
		return true;
	return tiles[x + y * width];
}

void World::updateTexture() {
	std::cout << "Drawing world to " << framebuffer << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width*tileSize, height*tileSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);	// Set a Color attachment to the frame buffer
	glDrawBuffer(GL_COLOR_ATTACHMENT0);										// Set a Color attachment to the draw buffer
	glViewport(0, 0, width*tileSize, height*tileSize);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	unsigned int shader = neutron::processor::LoadShader("res/shaders/world.shader");
	glUseProgram(shader);
	glBindVertexArray(neutron::passes::VAO_FLAT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, neutron::processor::getTextureFrom("res/textures/Tiles.png"));
	neutron::processor::uniM4(shader, "ortho", glm::ortho(0.f, 1.f*width*tileSize, 1.f*height*tileSize, 0.f));
	neutron::processor::uniF2(shader, "scale", glm::vec2(tileSize*1.f));
	neutron::processor::uniF2(shader, "uvsize", glm::vec2(1 / 8.f, 1 / 4.f));


	for (unsigned int i = 0; i < width; i++) {
		for (unsigned int j = 0; j < height; j++) {
			if (!get(i, j))
				continue;
			using namespace glm;
			ivec2 s = ivec2(0);
			if (get(i + 1, j))
				if (get(i - 1, j))
					if (get(i, j + 1))
						if (get(i, j - 1))
							if (!get(i - 1, j + 1))
								if (!get(i - 1, j - 1))
									if (!get(i + 1, j + 1))
										if (!get(i + 1, j - 1)) s = ivec2(7, 0);
										else s = ivec2(6, 3);
									else if (!get(i + 1, j - 1)) s = ivec2(6, 0);
									else s = ivec2(5, 0);
								else if (!get(i + 1, j + 1))
									if (!get(i + 1, j - 1)) s = ivec2(6, 2);
									else s = ivec2(5, 2);
								else if (!get(i + 1, j - 1)) s = ivec2(7, 1);
								else s = ivec2(4, 0);
							else if (!get(i - 1, j - 1))
								if (!get(i + 1, j + 1))
									if (!get(i + 1, j - 1)) s = ivec2(6, 1);
									else s = ivec2(7, 2);
								else if (!get(i + 1, j - 1)) s = ivec2(5, 1);
								else s = ivec2(4, 1);
							else if (!get(i + 1, j + 1))
								if (!get(i + 1, j - 1)) s = ivec2(5, 3);
								else s = ivec2(4, 2);
							else if (!get(i + 1, j - 1)) s = ivec2(4, 3);
							else s = ivec2(1);
						else s = ivec2(1, 0);
					else if (get(i, j - 1)) s = ivec2(1, 2);
					else s = ivec2(1, 3);
				else if (get(i, j + 1))
					if (get(i, j - 1)) s = ivec2(0, 1);
					else s = ivec2(0);
				else if (get(i, j - 1)) s = ivec2(0, 2);
				else s = ivec2(0, 3);
			else if (get(i - 1, j))
				if (get(i, j + 1))
					if (get(i, j - 1)) s = ivec2(2, 1);
					else s = ivec2(2, 0);
				else if (get(i, j - 1)) s = ivec2(2, 2);
				else s = ivec2(2, 3);
			else if (get(i, j + 1))
				if (get(i, j - 1)) s = ivec2(3, 1);
				else s = ivec2(3, 0);
			else if (get(i, j - 1)) s = ivec2(3, 2);
			else s = ivec2(3, 3);

			if(get(i, j) == 2)
				glBindTexture(GL_TEXTURE_2D, neutron::processor::getTextureFrom("res/textures/Tiles2.png"));
			else 
				glBindTexture(GL_TEXTURE_2D, neutron::processor::getTextureFrom("res/textures/Tiles.png"));
			neutron::processor::uniF2(shader, "texoff", fvec2(s));
			neutron::processor::uniF2(shader, "offset", vec2(i*tileSize, j*tileSize));
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, neutron::frame::width, neutron::frame::height);
}