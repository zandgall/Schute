#pragma once
#include "Quark.h"
#include "PageQuark.h"
#include "SliderQuark.h"
#include "ButtonQuark.h"
class TabQuark : public Quark {
public:
	static enum {
		// Look Type
		FLAT = 0, JARVOPIA = 1, OBJECTSONLY = 2, OUTLINE = 3,
		// Float
		CORNERROUNDING = LAST_FLOAT + 1, BEVEL = LAST_FLOAT + 2, TABROUNDING = LAST_FLOAT + 3, TABBEVEL = LAST_FLOAT+4
	};
	int lookType = FLAT;
	TabQuark() {
		
	}
	unsigned currentTab = 0;
	TabQuark(glm::vec3 position, glm::vec3 bounds, glm::vec3 contentBounds, std::string firstTab, unsigned int tabWidth, unsigned int tabHeight) {
		this->position = position+bounds*glm::vec3(.5); this->size = bounds*glm::vec3(.5); this->contentBounds = contentBounds;
		setF(CORNERROUNDING, 15.f);
		setF(BEVEL, 0.1f);
		setF(TABROUNDING, 15.f);
		setF(TABBEVEL, 0.1f);
		lookType = FLAT;
		setF4(COLOR, glm::vec4(0.5, 0.5, 0.5, 1));
		setB(GUI, true);
		tabs = std::vector<ButtonQuark*>();
		this->tabHeight = tabHeight;
		//glGenFramebuffers(1, &framebuffer);
		put(firstTab, tabWidth);
		tabs[0]->setOn(true);
	}
	~TabQuark();

	glm::vec2 contentBounds = glm::vec2();

	void put(std::string id, unsigned int tabWidth) {
		int nX = position.x + getF(CORNERROUNDING);
		for (int i = 0; i<tabs.size(); i++)
			nX += tabs[i]->size.x*2;
		ButtonQuark *dummy = new ButtonQuark(glm::vec3(nX, position.y-tabHeight, 0), glm::vec3(tabWidth, tabHeight+getF(TABROUNDING), 0));
		dummy->setS(TEXT, id);
		dummy->lookType = lookType;
		dummy->interactionType = ButtonQuark::TOGGLE;
		dummy->setF(ButtonQuark::CORNERROUNDING, getF(TABROUNDING));
		dummy->setF(ButtonQuark::BEVEL, getF(TABBEVEL));

		tabs.push_back(dummy);
		pages.push_back(new PageQuark(glm::vec3(0), size*2.f, glm::vec3(contentBounds, 0)));
		pages[pages.size() - 1]->setF(PageQuark::CORNERROUNDING, getF(CORNERROUNDING));
		pages[pages.size() - 1]->setF(PageQuark::BEVEL, getF(BEVEL));
	}
	
	void setTabHeight(int tabHeight) {
		this->tabHeight = tabHeight;
		for (int i = 0; i < tabs.size(); i++) {
			tabs[i]->size.y = tabHeight + getF(TABROUNDING);
			tabs[i]->position.y = -tabHeight + getF(TABBEVEL);
		}
	}

	ButtonQuark* getTabAt(size_t tab) {
		return tabs[tab];
	}

	PageQuark* getPageAt(size_t tab) {
		return pages[tab];
	}
	
	PageQuark* operator[](size_t tab) {
		return pages[tab];
	}

	void updateBounda(glm::vec3 bounds) {
		for (auto t = pages.begin(); t != pages.end(); t++)
			(*t)->updateBounda(bounds);
	}

	void setF(unsigned int id, float value) {
		Quark::setF(id, value);
		if (id == TABBEVEL)
			for (int i = 0; i < tabs.size(); i++)
				tabs[i]->setF(ButtonQuark::BEVEL, value);
		else if (id == TABROUNDING)
			for (int i = 0; i < tabs.size(); i++)
				tabs[i]->setF(ButtonQuark::CORNERROUNDING, value);
		else if (id == BEVEL)
			for (int i = 0; i < pages.size(); i++)
				pages[i]->setF(PageQuark::BEVEL, value);
		else if (id == CORNERROUNDING)
			for (int i = 0; i < pages.size(); i++)
				pages[i]->setF(PageQuark::CORNERROUNDING, value);
	}

	void setS(unsigned int id, const std::string &value) {
		Quark::setS(id, value);
		
		if(id==FONT)
			for (int i = 0; i < tabs.size(); i++)
				tabs[i]->setS(id, value);
	}

	void calculateAutoContentBounds();

	void tick();

	void render();

private: 
	void refreshTabs() {
		int nX = position.x + getF(CORNERROUNDING);
		for (int i = 0; i < tabs.size(); i++) {
			tabs[i]->position.x = nX;
			tabs[i]->position.y = position.y - tabHeight;
			nX += tabs[i]->size.x*2;
			tabs[i]->lookType = lookType;
			tabs[i]->setF(ButtonQuark::CORNERROUNDING, getF(TABROUNDING));
			tabs[i]->setF(ButtonQuark::BEVEL, getF(TABBEVEL));
		}
	}
	int tabHeight = 0;
	unsigned int framebuffer;
	std::vector<ButtonQuark*> tabs;
	std::vector<PageQuark*> pages;
	void _updateContentMask();
};

