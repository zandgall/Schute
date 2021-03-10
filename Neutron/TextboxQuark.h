#pragma once
#ifndef TEXTBOX_QUARK_H
#define TEXTBOX_QUARK_H

#include "Quark.h"
#include "MouseDetector.h"
#include "Neutron.h"
#include "NeutronFrame.h"
#include <algorithm>
#include <string>

class TextboxQuark : public Quark {
public:
	static enum {
		// Look type
		FLAT, JARVOPIA, TEXTONLY, OUTLINE,
		// Float4
		PRIMARYCOLOR = LAST_FLOAT4 + 1, LOCKEDCOLOR = LAST_FLOAT4 + 2,
		// Float4
		PRIMARYTEXTCOLOR = LAST_FLOAT4 + 3, INVERTEDTEXTCOLOR = LAST_FLOAT4 + 4,
		LAST_FLOAT4 = INVERTEDTEXTCOLOR,
		// Float
		CORNERROUNDING = LAST_FLOAT + 1, BEVEL = LAST_FLOAT + 2,
		LAST_FLOAT = BEVEL,
	};

	TextboxQuark() {}

	unsigned lookType = FLAT, cursor = 0, selectStart = 0;
	float fontSize = 24.f;

	std::u32string value, selection;

	//Constructor with Parameters
	TextboxQuark(glm::vec3 position, glm::vec3 bounds, std::u32string value = U"Enter here", float fontSize = 24.f, glm::vec3 rotation = glm::vec3(0)) : Quark(position + bounds * glm::vec3(0.5), 0.f, bounds* glm::vec3(0.5), rotation) {
		this->position.z = position.z;	// Keep the passed position's z value, but with changes to x and y based on size
		input = MouseDetector(position, position + bounds);	// Create a mouse detector, used for interaction
		
		prePos = this->position;	// Make a prePos variable to check if the user has tried to move the button this way
		preSize = size;				// Make a preSize variable, to check if the user has tried to rescale the button this way
		preRot = rotation;			// Make a preRot variable, to check if the user has tried to rotate the button this way

		lookType = FLAT;			// Default look type
		setB(GUI, true);			// Set GUI to true

		setColorPallete(glm::vec4(0.5, 0.5, 0.5, 1));	// Set default colors
		setTextPallete(glm::vec4(0, 0, 0, 1));			// For text too

		setF(CORNERROUNDING, 15.f);	// Default rounded corners,
		setF(BEVEL, 0.1f);			// bevel,
		//setS(TEXT, value);			// text,
		setS(FONT, "arial");		// and font		

		this->value = value;
		this->fontSize = fontSize;
		cursor = value.size();
		selectStart = value.size();

		if (neutron::frame::keys::charEvents.empty() || std::find(neutron::frame::keys::charEvents.begin(), neutron::frame::keys::charEvents.end(), &charEvent) == neutron::frame::keys::charEvents.end())
			neutron::frame::keys::addCharEvent(&charEvent);
		if (neutron::frame::keys::keyEvents.empty() || std::find(neutron::frame::keys::keyEvents.begin(), neutron::frame::keys::keyEvents.end(), &keyEvent) == neutron::frame::keys::keyEvents.end())
			neutron::frame::keys::addKeyEvent(&keyEvent);

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &frozenLocked);
		glGenTextures(1, &frozenNorm);
		glGenTextures(1, &lockedText);
		glGenTextures(1, &normText);
	}
	// Set the bounds and/or position
	void setBounds(glm::vec3 start, glm::vec3 end) {
		input.start = start;					// Change the input begin
		input.end = end;						// and end point,
		position = (start + end) * glm::vec3(0.5);// Move the position
		size = (end - start) * glm::vec3(0.5);	// Change the size,
		prePos = position;						// Update the prePos
		preSize = size;							// and preSize
	}
	// Set specifically the position
	void setPosition(glm::vec3 pos) {
		setBounds(pos - size, pos + size);	// Update bounds based on size

	}
	// Set the shape of the input
	void setShape(unsigned int type) {
		input.shape = type;
	}
	// Change if the button is locked or not
	void setLockedTo(bool locked) {
		input.locked = locked;
	}
	// Update the button and check for input
	void tick();
	// Render the button
	void render();
	// Set the color pallete based on one color
	void setColorPallete(glm::vec4 color) {
		setF4(PRIMARYCOLOR, color);
		setF4(LOCKEDCOLOR, color);
		frozen = false;
	}
	// Set the text's pallete based on one color
	void setTextPallete(glm::vec4 color) {
		setF4(PRIMARYTEXTCOLOR, color);
		setF4(INVERTEDTEXTCOLOR, glm::vec4(1 - color.x, 1-color.y, 1-color.z, 1));
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

	virtual void setS(unsigned int id, const std::string& value) {
		while (STRINGS.size() <= id)
			STRINGS.push_back("");
		STRINGS[id] = value;
		frozen = false;
	}

	unsigned int getFrozen() {
		return frozenNorm;
	}

	static void keyEvent(int key, int scancode, int action, int mods);
	static void charEvent(unsigned int key);
	static std::u32string ctrlChars;
	
	// Freeze all textures
	void freezeTextures() {
		set();
		freeze(&frozenNorm);
		freeze(&frozenLocked);
		freezeText(&normText);
		freezeText(&lockedText);
		frozen = true;
	}
private:
	glm::vec3 prePos = glm::vec3(0);
	glm::vec3 preSize = glm::vec3(0);
	glm::vec3 preRot = glm::vec3(0);
	MouseDetector input = MouseDetector(glm::vec3(0), glm::vec3(0));
	bool frozen = false;
	unsigned frozenNorm = 0, frozenLocked = 0, normText = 0, lockedText = 0, framebuffer = 0;
	int displayOffset = 0;

	// Freeze a texture
	void freeze(unsigned int* texture);
	void freezeText(unsigned int* text);
};
#endif