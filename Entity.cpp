#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include "Entity.h"
#include "Physics.h"
#include "Neutron/NeutronWorld.h"
#include <algorithm>
Entity::Entity() {
	mass = 50;
	strength = mass*70;
	elasticity = 0;
	hardness = 1;
	position = glm::vec2(0);
	velocity = glm::vec2(0);
	acceleration = glm::vec2(0);
	size = glm::vec2(1);
	pos = &position;
	vel = &velocity;
	acc = &acceleration;

	_vel = glm::vec2(0);
	genUUID();
}


Entity::~Entity() {
}

void Entity::force(glm::vec2 force) {
	acceleration += force / glm::vec2(mass);
}

void Entity::move(std::vector<Entity*> *list, char checker(int, int), double timespeed) {

	timespeed = (std::min)(timespeed, 0.02);

	_vel += acceleration / (float)(timespeed);
	_vel += (neutron::world::camera.WorldUp.xy * (float)(GRAVITY));

	velocity += acceleration;
	velocity += (neutron::world::camera.WorldUp.xy*(float) (GRAVITY*timespeed));
	
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

	P::field.addWeight(glm::vec4(position + glm::vec2(0, size.y), glm::vec2(size.x, velocity.y)), _vel.y*mass, UUID);
	
	collide(list, checker, timespeed);
	if (vel->x == 0)
		_vel.x = 0;
	if (vel->y == 0)
		_vel.y = 0;
	float l = velocity.x < 0 ? -1 : 1;
	velocity.x -= l * std::min(GRAVITY * (aabb.w * 0.005f + 0.005f) * mass * float(timespeed), abs(velocity.x));
	_vel.x -= l * std::min(GRAVITY * (aabb.w * 0.005f + 0.005f) * mass * float(timespeed), abs(_vel.x));

	position += velocity*float(timespeed);
	acceleration = glm::vec2(0);
}

void cE(Entity *self, Entity *to, double timespeed) {
	using namespace glm;
	if (AABB(self->position + vec2(self->vel->x*timespeed, 0.1), vec2(0, self->size.y-0.2), to->position, to->size)) {
		self->aabb.x = 2;
		self->position.x = to->position.x + to->size.x;
		self->vel->x = (std::max)(0.0f, -self->vel->x*self->elasticity);
		//self->vel->x += (std::min)(1/(1/self->hardness + 1/to->hardness) * float(timespeed), abs(-self->vel->x * self->elasticity));
		//self->vel->x += (std::min)(to->hardness * abs(self->vel->x) * float(timespeed), abs(self->vel->x) * (self->elasticity + 1));
	}
	if (AABB(self->position + vec2(self->size.x + self->vel->x*timespeed, 0.1), vec2(0, self->size.y-0.2), to->position, to->size)) {
		self->aabb.z = 2;
		self->position.x = to->position.x - self->size.x;
		self->vel->x = (std::min)(0.0f, -self->vel->x*self->elasticity);
		//self->vel->x -= (std::min)(1 / (1 / self->hardness + 1 / to->hardness) * float(timespeed), abs(-self->vel->x * self->elasticity));
		//self->vel->x -= (std::min)(to->hardness * abs(self->vel->x) * float(timespeed), abs(self->vel->x) * (self->elasticity + 1));
	}
	if (AABB(self->position + vec2(0.1, self->vel->y*timespeed), vec2(self->size.x-0.2, 0), to->position, to->size)) {
		self->aabb.y = 2;
		self->position.y = to->position.y + to->size.y;
		self->vel->y = (std::max)(0.0f, -self->vel->y*self->elasticity);
		//self->vel->y += (std::min)(1 / (1 / self->hardness + 1 / to->hardness) * float(timespeed), abs(-self->vel->y * self->elasticity));
		//self->vel->y += (std::min)(to->hardness * abs(self->vel->y) * float(timespeed), abs(self->vel->y) * (self->elasticity + 1));
	}
	if (AABB(self->position + vec2(0.1, self->size.y + self->vel->y*timespeed), vec2(self->size.x-0.2, 0), to->position, to->size)) {
		self->aabb.w = 2;
		self->position.y = to->position.y - self->size.y;
		self->vel->y = (std::min)(0.0f, -self->vel->y*self->elasticity);
		//self->vel->x -= (std::min)(1 / (1 / self->hardness + 1 / to->hardness) * float(timespeed), abs(-self->vel->y * self->elasticity));
		//self->vel->y -= (std::min)(to->hardness * abs(self->vel->y) * float(timespeed), abs(self->vel->y) * (self->elasticity + 1));
	}
}

void Entity::collide(std::vector<Entity*> *list, char c(int, int), double timespeed) {
	glm::vec4 p = glm::vec4(pos->x + vel->x * timespeed, pos->y + vel->y * timespeed, pos->x + vel->x * timespeed + size.x, pos->y + vel->y * timespeed + size.y);
	glm::vec2 n = glm::vec2(pos->x, pos->x + size.x);

	// Check bottom
	aabb.w = (std::max)(c(floor(n.x + 0.1), floor(p.w)), c(floor(n.y - 0.1), floor(p.w)));
	for (int i = 1; i < size.x - 1.1 && !aabb.w; i++) { // Starting one tile from the x position, until it reaches the points already checked, but break if already found ground
		aabb.w = (std::max)((char)aabb.w, c(floor(n.x + i), floor(p.w)));
	}
	if (aabb.w) {// If colliding with bottom,
		position.y = floor(p.w) - size.y;
		velocity.y = (std::min)(0.0f, -velocity.y*elasticity);
		p.y = pos->y + vel->y * timespeed;
		p.w = pos->y + vel->y * timespeed + size.y;
	}

	// Check top
	aabb.y = (std::max)(c(floor(n.x + 0.1), floor(p.y)), c(floor(n.y - 0.1), floor(p.y)));
	for (int i = 1; i < size.x - 1.1 && !aabb.y; i++) { // Starting one tile from the x position, until it reaches the points already checked, but break if already found ceiling
		aabb.y = (std::max)((char)aabb.y, c(floor(n.x + i), floor(p.y)));
	}
	if (aabb.y) {// If colliding with top,
		position.y = floor(p.y)+1;
		velocity.y = (std::max)(0.0f, -velocity.y*elasticity);
		p.y = pos->y + vel->y * timespeed;
		p.w = pos->y + vel->y * timespeed + size.y;
	}

	// Check left
	aabb.x = (std::max)(c(floor(p.x), floor(p.y+0.1)), c(floor(p.x), floor(p.w-0.1)));
	for (int i = 1; i < size.y - 1.1 && !aabb.x; i++) { // Starting one tile from the x position, until it reaches the points already checked, but break if already found ceiling
		aabb.x = (std::max)((char)aabb.x, c(floor(p.x), floor(p.y+i)));
	}
	if (aabb.x) {// If colliding with left,
		position.x = floor(p.x) + 1;
		velocity.x = (std::max)(0.0f, -velocity.x*elasticity);
		p.x = pos->x + vel->x * timespeed;
		p.z = pos->x + vel->x * timespeed + size.x;
	}

	// Check right
	aabb.z = (std::max)(c(floor(p.z), floor(p.y + 0.1)), c(floor(p.z), floor(p.w - 0.1)));
	for (int i = 1; i < size.y - 1.1 && !aabb.x; i++) { // Starting one tile from the x position, until it reaches the points already checked, but break if already found ceiling
		aabb.z = (std::max)((char)aabb.z, c(floor(p.z), floor(p.y + i)));
	}
	if (aabb.z) {// If colliding with right,
		position.x = floor(p.z) - size.x;
		velocity.x = (std::min)(0.0f, -velocity.x*elasticity);
	}

	std::vector<Entity*> solids; 
	std::copy_if(list->begin(), list->end(), std::back_inserter(solids), [](Entity* e) {return e->solid; });
	for (auto e : solids)
		if(e!=this)
			cE(this, e, timespeed);
}

/*
void Entity::collide(bool check(int, int)) {
	aabb = glm::bvec4(false);
	contact = glm::bvec4(false);

	collisions.x = ceil(pos->x + velocity.x);				// Left Position
	collisions.y = ceil(pos->y + velocity.y);				// Top Position
	collisions.z = floor(pos->x + size.x + velocity.x);		// Right Position
	collisions.w = floor(pos->y + size.y + velocity.y);		// Bottom Position


	int aX = floor(pos->x + velocity.x);
	int aY = floor(pos->y + velocity.y);
	int bX = floor(pos->x + size.x + velocity.x);
	int bY = floor(pos->y + size.y + velocity.y);
	int xX = floor(pos->x + 0.2 + velocity.x);
	int xY = floor(pos->y + 0.2 + velocity.y);
	int zX = floor(pos->x + size.x - 0.2 + velocity.x);
	int zY = floor(pos->y + size.y - 0.2 + velocity.y);
	aabb.x = check(aX, xY);									// Left Collision
	aabb.x = aabb.x || check(aX, zY);						// Left Collision
	contact.x = pos->x == collisions.x && aabb.x;			// Left Contact			(Is currently right up against the tile)
	aabb.y = check(xX, aY);									// Top Collision
	aabb.y = aabb.y || check(zX, aY);						// Top Collision
	contact.y = pos->y == collisions.y && aabb.y;			// Top Contact
	aabb.z = check(bX, xY);									// Right Collision
	aabb.z = aabb.z || check(bX, zY);						// Right Collision
	contact.z = pos->x + size.x == collisions.z && aabb.z;	// Right Contact
	aabb.w = check(xX, bY);									// Bottom Collision
	aabb.w = aabb.w || check(zX, bY);						// Bottom Collision
	contact.w = pos->y + size.y == collisions.w && aabb.w;	// Bottom Contact
}
*/