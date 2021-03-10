#pragma once
#include "Quark.h"
#include "Neutron.h"
#include <glm/glm.hpp>
#include "MouseDetector.h"

class SliderQuark : public Quark {
public:
	static enum {
		// Orientation
		VERTICAL = 0, HORIZONTAL = 1,
		// Look Type
		FLAT, JARVOPIA,
		// Float4
		PRIMARYCOLOR = LAST_FLOAT4 + 1, LOCKEDCOLOR = LAST_FLOAT4 + 2, HOVEREDCOLOR = LAST_FLOAT4 + 3, DRAGGEDCOLOR = LAST_FLOAT4 + 4, BODYCOLOR = LAST_FLOAT4 + 5,
		//Float
		CORNERROUNDING = LAST_FLOAT + 1, BEVEL = LAST_FLOAT + 2,
	};
	
	unsigned char lookType; 
	unsigned char orientation = 1;

	double value = 0, maxVal, minVal;

	SliderQuark();
	SliderQuark(glm::vec3 position, glm::vec3 bounds, double minValue, double maxValue, double standardValue, unsigned const char orientation = HORIZONTAL);
	
	void setBodyBounds(glm::vec3 start, glm::vec3 end);
	void changeKnobBounds(glm::vec3 start, glm::vec3 size);
	void setPosition(glm::vec3 pos);
	void changeKnobRounding(float knobRounding);
	void setLockedTo(bool locked);
	void tick();
	void render();
	void setColorPallete(glm::vec4 color);

	void useClassicJarvopiaTemplate();
private:
	MouseDetector body = MouseDetector(glm::vec3(0), glm::vec3(0)), knob = MouseDetector(glm::vec3(0), glm::vec3(0));
	double prV = 0;
	int strafe = 0;
	void updateKnobConstraints() {
		if (orientation == 1) {
			glm::vec3 n = glm::vec3(position.x - size.x, knob.start.y, 0);
			glm::vec3 m = glm::vec3(position.x + size.x, knob.end.y, 0);
			knob.constrain(n, m);
		}
		else {
			glm::vec3 n = glm::vec3(knob.start.x, position.y - size.y, 0);
			glm::vec3 m = glm::vec3(knob.end.x, position.y + size.y, 0);
			knob.constrain(n, m);
		}
	}
};

