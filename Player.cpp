#include "Player.h"
#include "Physics.h"
#include "Neutron/NeutronWorld.h"
#include <algorithm>
#include "Neutron/Neutron.h"
#include "Neutron/NeutronPasses.h"
#include "Neutron/NeutronModes.h"
#include "Neutron/NeutronProcessor.h"
#include "Neutron/NeutronFrame.h"
#include <glm/gtx/string_cast.hpp>
#include "Vine.h"

using namespace neutron;
using namespace frame;

Player::Player() : Entity() {
	size = glm::vec2(0.4, 0.8);
	if(isSet()) {
		unsigned int s = processor::getTextureFrom("res/textures/player.png", GL_NEAREST);
		tex = std::vector<unsigned int>(); 
		tex.push_back(processor::getCroppedTexture(s, 0, 0, 16, 16));
		tex.push_back(processor::getCroppedTexture(s, 16, 0, 16, 16));
		tex.push_back(processor::getCroppedTexture(s, 32, 0, 16, 16));
		tex.push_back(processor::getCroppedTexture(s, 48, 0, 16, 16));
		tex.push_back(processor::getCroppedTexture(s, 0, 16, 16, 16));
		tex.push_back(processor::getCroppedTexture(s, 16, 16, 16, 16));
		tex.push_back(processor::getCroppedTexture(s, 32, 16, 16, 16));
		tex.push_back(processor::getCroppedTexture(s, 48, 16, 16, 16));
		tex.push_back(processor::getCroppedTexture(s, 0, 32, 16, 16));
		tex.push_back(processor::getCroppedTexture(s, 16, 32, 16, 16));
		tex.push_back(processor::getCroppedTexture(s, 0, 48, 16, 16));
		tex.push_back(processor::getCroppedTexture(s, 16, 48, 16, 16));
	}
}

Player::~Player() {

}

unsigned int Player::getFrame() {
	if (climbing)
		return tex[8 + (int)std::fmod(movetime, 2)];
	if (glm::distance(glm::vec2(0), velocity)>0.1)
		return tex[4 + (int)std::fmod(movetime, 4)];
	return tex[2];
}

void Player::move(std::vector<Entity*> *list, char checker(int, int), double timespeed) {
	timespeed = (std::min)(timespeed, 0.02);

	_vel += acceleration / (float)(timespeed);
	if(!climbing)
		_vel += (neutron::world::camera.WorldUp.xy * (float)(GRAVITY));

	velocity += acceleration;
	if (!climbing)
		velocity += (neutron::world::camera.WorldUp.xy*(float)(GRAVITY*timespeed));

	P::field.clearForces(UUID);
	P::field.clearWeights(UUID);

	// Apply to force field
	if (velocity.x > 0) {
		P::field.addForce(glm::vec4(position + glm::vec2(size.x, 0), glm::vec2(velocity.x * timespeed, size.y)), glm::vec2(_vel.x, 0) * mass, UUID);
		if (velocity.y > 0)
			P::field.addForce(glm::vec4(position + glm::vec2(velocity.x * timespeed, size.y), glm::vec2(size.x, velocity.y * timespeed)), _vel * mass, UUID);
		else
			P::field.addForce(glm::vec4(position + glm::vec2(velocity.x * timespeed, velocity.y * timespeed), glm::vec2(size.x, -velocity.y * timespeed)), _vel * mass, UUID);
	}
	else {
		P::field.addForce(glm::vec4(position + glm::vec2(velocity.x * timespeed, 0), glm::vec2(-velocity.x * timespeed, size.y)), glm::vec2(_vel.x, 0) * mass, UUID);
		if (velocity.y > 0)
			P::field.addForce(glm::vec4(position + glm::vec2(velocity.x * timespeed, size.y), glm::vec2(size.x, velocity.y * timespeed)), _vel * mass, UUID);
		else
			P::field.addForce(glm::vec4(position + glm::vec2(velocity.x * timespeed, velocity.y * timespeed), glm::vec2(size.x, -velocity.y * timespeed)), _vel * mass, UUID);
	}

	P::field.addWeight(glm::vec4(position + glm::vec2(0, size.y), glm::vec2(size.x, velocity.y)), _vel.y * mass, UUID);

	collide(list, checker, timespeed);
	if (vel->x == 0)
		_vel.x = 0;
	if (vel->y == 0)
		_vel.y = 0;
	float l = velocity.x < 0 ? -1 : 1;
	velocity.x -= l * std::min(GRAVITY * ((aabb.w||climbing) * 0.005f + 0.005f) * mass * float(timespeed), abs(velocity.x));
	_vel.x -= l * std::min(GRAVITY * ((aabb.w || climbing) * 0.005f + 0.005f) * mass * float(timespeed), abs(_vel.x));
	if (climbing) {
		l = velocity.y < 0 ? -1 : 1;
		velocity.y -= l * std::min(GRAVITY * (0.01f) * mass * float(timespeed), abs(velocity.y));
		_vel.y -= l * std::min(GRAVITY * (0.01f) * mass * float(timespeed), abs(_vel.y));
	}
	position += velocity * float(timespeed);
	acceleration = glm::vec2(0);
}

void Player::tick(std::vector<Entity*> *list, double timespeed) {
	bool preclimbing = climbing;
	climbing = false;
	Vine *current = nullptr;
	for (Entity *e : *list)
		if (dynamic_cast<Vine*>(e) != NULL) {
			climbing = climbing || (((keys::keys[GLFW_KEY_UP] || keys::keys[GLFW_KEY_W] && !aabb.w) || preclimbing) && AABB(e->position, e->size, position, size));
			if(AABB(e->position, e->size, position, size))
				current = dynamic_cast<Vine*>(e);
		}
	if (keys::keys[GLFW_KEY_UP] || keys::keys[GLFW_KEY_W]) {
		if(climbing)
			force(glm::vec2(0, -strength * timespeed));
		else
			force(glm::vec2(0, ((aabb.w>0) * 0.35 + (timespeed * ((vel->y<0) * 0.25 + 0.2))) * -strength));
		flip.y = 1;
		climbRot = -glm::pi<float>()/2;
	}
	if (keys::keys[GLFW_KEY_LEFT] || keys::keys[GLFW_KEY_A]) {
		force(glm::vec2(-strength * timespeed, 0));
		flip.x = -1;
		climbRot = glm::pi<float>();
	}
	if (keys::keys[GLFW_KEY_RIGHT] || keys::keys[GLFW_KEY_D]) {
		force(glm::vec2(strength * timespeed, 0));
		flip.x = 1;
		climbRot = 0;
	}
	if (keys::keys[GLFW_KEY_DOWN] || keys::keys[GLFW_KEY_S]) {
		force(glm::vec2(0, strength * timespeed));
		flip.y = -1;
		climbRot = glm::pi<float>() / 2;
	}

	movetime += glm::distance(glm::vec2(0), velocity)*timespeed;

	if (!climbing)
		flip.y = 1;

	if (keys::keys[GLFW_KEY_R])
		position = spawn;

	if (keys::keys[GLFW_KEY_SPACE] && frame::keys::typed && (climbing || aabb.x || aabb.y || aabb.z || aabb.w)) {
		Vine *vine = new Vine();
		bool cancel = false;
		vine->position = glm::floor(this->position+glm::vec2(0.25));
		if (climbing) {
			vine->branch = true;
			if (current->vertical) {
				vine->position.x = current->position.x + 0.5;
				if (keys::keys[GLFW_KEY_LEFT] || keys::keys[GLFW_KEY_A]) {
					vine->rotation = -glm::pi<double>() / 2;
				} 
				else if (keys::keys[GLFW_KEY_RIGHT] || keys::keys[GLFW_KEY_D]) {
					vine->rotation = glm::pi<double>() / 2;
				}
				else cancel = true;
			}
			else {
				vine->position.y = current->position.y + 0.5;
				if (keys::keys[GLFW_KEY_DOWN] || keys::keys[GLFW_KEY_S]) {
					vine->rotation = glm::pi<double>();
				} 
				else if (keys::keys[GLFW_KEY_UP] || keys::keys[GLFW_KEY_W]) {
					vine->rotation = 0;
				}
				else cancel = true;
			}
		}
		else {
			if (aabb.y) {
				vine->rotation = glm::pi<double>();
				cancel = aabb.y == 2;
			}
			else if (aabb.x) {
				vine->rotation = glm::pi<double>() / 2;
				cancel = aabb.x == 2;
			}
			else if (aabb.z) {
				vine->rotation = -glm::pi<double>() / 2;
				vine->position.x++;
				cancel = aabb.z == 2;
			}
			else {
				vine->rotation = 0;
				vine->position.y++;
				cancel = aabb.w == 2;
			}
		}

		vine->vertical = (vine->rotation / glm::pi<double>() == 0 || vine->rotation / glm::pi<double>() == 1);

		if (!cancel)
			list->insert(list->begin(), vine);
	}
}

void Player::render() {
	modes::gui = true;
	if (climbing) {
		passes::transform = processor::form(glm::vec3(position + size / 2.f, 0), glm::vec3(0.5f*size.yy, 0), glm::vec3(0, 0, climbRot));
		draw(getFrame());
	}
	else {
		passes::transform = processor::form(glm::vec3(position + size / 2.f, 0), glm::vec3(0.5f*size.yy*glm::vec2(flip), 0));
		draw(getFrame());
		passes::transform = processor::form(glm::vec3(position + size / 2.f + velocity * glm::vec2(0.005), 0), glm::vec3(0.5f*size.yy*glm::vec2(flip), 0));
		draw(tex[(int)std::fmod(glfwGetTime(), 2)]);
	}
	if (false) {
		passes::transform = processor::rect(pos->x - 0.1, pos->y, 0.1, size.y);
		draw(0, glm::vec4(!aabb.x, aabb.x, 0, 1));
		passes::transform = processor::rect(pos->x, pos->y - 0.1, size.x, 0.1);
		draw(0, glm::vec4(!aabb.y, aabb.y, 0, 1));
		passes::transform = processor::rect(pos->x + size.x, pos->y, 0.1, size.y);
		draw(0, glm::vec4(!aabb.z, aabb.z, 0, 1));
		passes::transform = processor::rect(pos->x, pos->y + size.y, size.x, 0.1);
		draw(0, glm::vec4(!aabb.w, aabb.w, 0, 1));
	}
}