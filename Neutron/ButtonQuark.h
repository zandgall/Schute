#pragma once
#ifndef BUTTONQUARK_H
#define BUTTONQUARK_H

#include "Quark.h"
#include "Neutron.h"
#include "MouseDetector.h"
#include <iostream>
using namespace neutron;
class ButtonQuark : public Quark {
public:
	static enum {
		// Interaction Type
		STANDARD, TOGGLE, 
		// Look Type
		FLAT = 0, JARVOPIA = 1, TEXTONLY = 2, OUTLINE = 3,
		// Shape
		RECT = 0, ELLIPSE = 1,
		// Float4
		PRIMARYCOLOR = LAST_FLOAT4 + 1, LOCKEDCOLOR = LAST_FLOAT4+2, HOVEREDCOLOR = LAST_FLOAT4+3, ONCOLOR = LAST_FLOAT4+4, HOVEREDONCOLOR = LAST_FLOAT4+5,		
		// Float4
		PRIMARYTEXTCOLOR = LAST_FLOAT4 + 6, LOCKEDTEXTCOLOR = LAST_FLOAT4 + 7, HOVEREDTEXTCOLOR = LAST_FLOAT4 + 8, ONTEXTCOLOR = LAST_FLOAT4 + 9, HOVEREDONTEXTCOLOR = LAST_FLOAT4 + 10,
		LAST_FLOAT4 = HOVEREDONTEXTCOLOR,
		// Float
		CORNERROUNDING = LAST_FLOAT+1, BEVEL = LAST_FLOAT + 2,
		LAST_FLOAT = BEVEL,
	};
	// Whether or not the button has been pressed, or is toggled
	bool on = false;
	// How the button will render
	unsigned char lookType;
	// How the button will be interacted with
	unsigned char interactionType;

	// Empty Constructor
	ButtonQuark() {

	}
	//Constructor with Parameters
	ButtonQuark(glm::vec3 position, glm::vec3 bounds, glm::vec3 rotation = glm::vec3(0)) : Quark(position+bounds*glm::vec3(0.5), 0.f, bounds*glm::vec3(0.5), rotation) {
		this->position.z = position.z;	// Keep the passed position's z value, but with changes to x and y based on size
		input = MouseDetector(position, position + bounds);	// Create a mouse detector, used for interaction

		prePos = this->position;	// Make a prePos variable to check if the user has tried to move the button this way
		preSize = size;				// Make a preSize variable, to check if the user has tried to rescale the button this way
		preRot = rotation;			// Make a preRot variable, to check if the user has tried to rotate the button this way
		
		lookType = FLAT;			// Default look type
		interactionType = STANDARD;	// and interaction type
		setB(GUI, true);			// Set GUI to true
		
		setColorPallete(glm::vec4(0.5, 0.5, 0.5, 1));	// Set default colors
		setTextPallete(glm::vec4(0, 0, 0, 1));			// For text too

		setF(CORNERROUNDING, 15.f);	// Default rounded corners,
		setF(BEVEL, 0.1f);			// bevel,
		setS(TEXT, "Sample Text");	// text,
		setS(FONT, "arial");		// and font

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &frozenHovered);
		glGenTextures(1, &frozenLocked);
		glGenTextures(1, &frozenNorm);
		glGenTextures(1, &frozenOn);
		glGenTextures(1, &frozenToggled);
	}
	// Set the bounds and/or position
	void setBounds(glm::vec3 start, glm::vec3 end){
		input.start = start;					// Change the input begin
		input.end = end;						// and end point,
		position = (start + end)*glm::vec3(0.5);// Move the position
		size = (end - start)*glm::vec3(0.5);	// Change the size,
		prePos = position;						// Update the prePos
		preSize = size;							// and preSize
	}
	// Set specifically the position
	void setPosition(glm::vec3 pos) {
		setBounds(pos-size, pos+size);	// Update bounds based on size

	}
	// Set the shape of the input
	void setShape(unsigned int type) {
		input.shape = type;
	}
	// Change if the button is locked or not
	void setLockedTo(bool locked) {
		input.locked = locked;
	}
	void setOn(bool on) {
		this->on = on;
		displayOn = on * 10;
	}
	// Update the button and check for input
	void tick();
	// Render the button
	void render();
	// Set the "on clicked" function
	void onClickedDo(void(*function)()) {
		onClicked = function;
	}
	// Set the color pallete based on one color
	void setColorPallete(glm::vec4 color) {
		setF4(PRIMARYCOLOR, color);
		setF4(LOCKEDCOLOR, color);
		setF4(HOVEREDCOLOR, color);
		setF4(ONCOLOR, color);
		setF4(HOVEREDONCOLOR, color);
		frozen = false;
	}
	// Set the text's pallete based on one color
	void setTextPallete(glm::vec4 color) {
		setF4(PRIMARYTEXTCOLOR, color);
		setF4(LOCKEDTEXTCOLOR, color);
		setF4(HOVEREDTEXTCOLOR, color);
		setF4(ONTEXTCOLOR, color);
		setF4(HOVEREDONTEXTCOLOR, color);
		frozen = false;
	}

	// Whenever someone changes something, unfreeze the button in order to apply the change

	virtual void setVao(unsigned int value, unsigned int vertCount) {
		while (UINTS.size() <= VAO)
			UINTS.push_back(0);
		UINTS[VAO] = value;
		this->vertCount = vertCount;
		frozen = false;
	}

	virtual void setUINT(unsigned int id, unsigned int value) {
		while (UINTS.size() <= id)
			UINTS.push_back(0);
		if (id == VAO) {
			std::cout << "SET VAO THROUGH SETVAO" << std::endl;
			throw EXCEPTION_INVALID_HANDLE;
		}

		UINTS[id] = value;
		frozen = false;
	}

	virtual void setF(unsigned int id, float value) {
		while (FLOATS.size() <= id)
			FLOATS.push_back(0);
		FLOATS[id] = value;
		frozen = false;
	}

	virtual void setF4(unsigned int id, glm::vec4 value) {
		while (FLOAT4S.size() <= id)
			FLOAT4S.push_back(glm::vec4(0));
		FLOAT4S[id] = value;
		frozen = false;
	}

	virtual void setB(unsigned int id, bool value) {
		while (BOOLS.size() <= id)
			BOOLS.push_back(false);
		BOOLS[id] = value;
		frozen = false;
	}

	virtual void setM4(unsigned int id, glm::mat4 value) {
		while (MATS.size() <= id)
			MATS.push_back(glm::mat4(0));
		MATS[id] = value;
		frozen = false;
	}

	virtual void setS(unsigned int id, const std::string &value) {
		while (STRINGS.size() <= id)
			STRINGS.push_back("");
		STRINGS[id] = value;
		frozen = false;
	}

	unsigned int getFrozen() {
		return frozenNorm;
	}
private:
	glm::vec3 prePos = glm::vec3(0);
	glm::vec3 preSize = glm::vec3(0);
	glm::vec3 preRot = glm::vec3(0);
	MouseDetector input = MouseDetector(glm::vec3(0), glm::vec3(0));
	void(*onClicked)() = nullptr;
	bool frozen = false;
	unsigned int frozenNorm = 0, frozenLocked = 0, frozenToggled = 0, frozenOn = 0, frozenHovered = 0, framebuffer = 0;
	int displayOn = 0;

	// Freeze a texture
	void freeze(unsigned int *texture);

	// Freeze all textures
	void freezeTextures() {
		set();
		freeze(&frozenNorm);
		freeze(&frozenHovered);
		freeze(&frozenLocked);
		freeze(&frozenOn);
		freeze(&frozenToggled);
		frozen = true;
	}

};

#endif