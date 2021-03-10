#include "Rock.h"

#include "Neutron/NeutronWorld.h"
#include "Neutron/NeutronPasses.h"
#include "Neutron/NeutronModes.h"
#include "Neutron/Neutron.h"
#include "Neutron/NeutronProcessor.h"
using namespace neutron;

Rock::Rock(): Entity() {
	size = glm::vec2(3);
	solid = true;
}


Rock::~Rock()
{
}

void Rock::render() {
	modes::gui = true;
	passes::transform = processor::rect(pos->x, pos->y, size.x, size.y);
	draw(processor::getTextureFrom("res/textures/rock.png", GL_NEAREST));
}
