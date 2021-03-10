#include "NeutronProcessor.h"
#include "TabQuark.h"
#include "NeutronPasses.h"
#include "NeutronFrame.h"
#include "NeutronModes.h"
#include "Neutron.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
using namespace neutron;
using namespace neutron::frame;
TabQuark::~TabQuark() {}

void TabQuark::render() {
	for (int i = 0; i < tabs.size(); i++) {
		tabs[i]->render();
	}
	pages[currentTab]->position = position+size;
	pages[currentTab]->render();
}

void TabQuark::tick() {
	int pret = currentTab;
	for (int t = 0; t < tabs.size(); t++) {
		if (mouse::l)
			tabs[t]->setOn(false);
		tabs[t]->tick();
		if (tabs[t]->on)
			currentTab = t;
	}
	if (pret == currentTab && mouse::l) {
		tabs[pret]->setOn(true);
	}
	pages[currentTab]->tick();
}

void TabQuark::calculateAutoContentBounds() {
	contentBounds = glm::vec2(0);
	for (auto q = pages.begin(); q != pages.end(); q++) {
		(*q)->calculateAutoContentBounds();
		auto s = (*q)->contentBounds;
		contentBounds.x = max(contentBounds.x, s.x);
		contentBounds.y = max(contentBounds.y, s.y);
		(*q)->contentBounds = contentBounds;
	}
}

void TabQuark::_updateContentMask() {
	for (auto q = pages.begin(); q != pages.end(); q++)
		(*q)->updateContentMask();
}