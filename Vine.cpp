#include "Vine.h"
#include "Rock.h"
#include "Neutron/Neutron.h"
#include "Neutron/NeutronWorld.h"
#include "Neutron/NeutronFrame.h"
#include "Neutron/NeutronProcessor.h"
#include "Neutron/NeutronPasses.h"
#include "Neutron/SerialImage.h"
#include "Physics.h"

unsigned int vinetex = 0, vinetex2 = 0, headtex = 0, headtex2 = 0, headtex3 = 0, headtex4 = 0;

Vine::Vine() : Entity() {
	this->growing = true;
	if (!vinetex) {
		unsigned int texture = neutron::processor::getTextureFrom("res/textures/Vines.png", GL_NEAREST, GL_CLAMP_TO_EDGE);
		vinetex = neutron::processor::getCroppedTexture(texture, 0, 0, 16, 16);
		vinetex2 = neutron::processor::getCroppedTexture(texture, 16, 16, 16, 16);
		glBindTexture(GL_TEXTURE_2D, vinetex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindTexture(GL_TEXTURE_2D, vinetex2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		headtex = neutron::processor::getCroppedTexture(texture, 48, 48, 16, 16);
		glBindTexture(GL_TEXTURE_2D, headtex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		headtex2 = neutron::processor::getCroppedTexture(texture, 48, 32, 16, 16);
		glBindTexture(GL_TEXTURE_2D, headtex2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		headtex3 = neutron::processor::getCroppedTexture(texture, 48, 16, 16, 16);
		glBindTexture(GL_TEXTURE_2D, headtex3);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		headtex4 = neutron::processor::getCroppedTexture(texture, 48, 0, 16, 16);
		glBindTexture(GL_TEXTURE_2D, headtex4);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}


Vine::~Vine() {

}

void Vine::tick(std::vector<Entity*> *list, double time) {
	this->time += time*16*growing;
	if (growth == 1) {
		start = position;
	}
}

void Vine::move(std::vector<Entity*> *list, char checker(int, int), double timespeed) {
	dir = glm::vec2(sin(rotation), -cos(rotation));
	glm::vec2 top = start + (dir * float(growth+timespeed*4));
	if (growing && checker(floor(top.x), floor(top.y))) {
		growing = false;
		growth = floor(growth + timespeed * 4) + branch/2.f;
	} 
	else if (growing)
		growth += timespeed * 4;
	position = glm::vec2(std::min(start.x, start.x + float(growth) * dir.x), std::min(start.y, start.y + float(growth) * dir.y));
	size = glm::vec2(std::abs(float(growth)*dir.x) + vertical, std::abs(float(growth) * dir.y) + !vertical);
	if(growing)
		for (Entity* e : *list)
			if (AABB(e->position, e->size, position, size) && dynamic_cast<Rock*>(e) != NULL)
				e->position += dir * float(timespeed*4.0) * glm::vec2((e->aabb.w>0)*((dir.x<0 && e->aabb.x==0)||(dir.x>0 && e->aabb.z==0)), ((dir.y < 0 && e->aabb.y == 0) || (dir.y > 0 && e->aabb.w==0)));
}

void Vine::render() {
	using namespace neutron;
	passes::shader = processor::LoadShader("res/shaders/vineHead.shader");

	passes::cust_mat4s["rotate"] = glm::mat4(1);
	passes::transform = glm::scale(processor::rect(pos->x + 0.5*(growing && (dir.x == -1)), pos->y + 0.5*(growing && (dir.y == -1)), (size.x - 0.5*(growing && !vertical)), (size.y - 0.5*(growing&&vertical))), glm::vec3(dir.x < 0 ? -1 : 1, dir.y < 0 ? -1 : 1, 1));
	passes::uvTransform = processor::scaled(glm::vec3(size.x - 0.5 + 0.5*(vertical || !growing), size.y - 0.5 + 0.5 * (!vertical || !growing), 0));
	draw(vertical ? vinetex : vinetex2);

	glm::vec2 offset = start + (dir*float(growth));
	passes::uvTransform = glm::mat4(1);
	passes::transform = processor::rect(offset.x - (growing && !vertical) + (dir.x==-1), offset.y - (growing && vertical) + (dir.y == -1), 1, 1, glm::vec3(0, 0, 0));
	passes::cust_mat4s["rotate"] = processor::rotation(glm::vec3(0, 0, sin(time * 5.09295818 / glm::pi<double>()) / 4.0 * ((dir.x==1 || dir.y == -1) ? -1 : 1)));
	passes::cust_float4s["dir"] = glm::vec4(dir.x/2, dir.y/2, 0, 0);
	
	if (growing)
		draw(dir.y == -1 ? headtex : dir.y == 1 ? headtex4 : dir.x == 1 ? headtex3 : headtex2);
	passes::shader = passes::SHADER_FLAT;
}