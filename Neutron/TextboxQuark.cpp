#include "TextboxQuark.h"
#include "NeutronPasses.h"
#include "NeutronFrame.h"
#include "NeutronProcessor.h"
#include "NeutronModes.h"
#include "NeutronWorld.h"
#include <windows.h>
#include <codecvt>
using namespace neutron;

std::u32string TextboxQuark::ctrlChars = U" ,(){}[]\\\"\'~`<>*+-=_!@#$%^&:;/.";

void TextboxQuark::tick() {
	input.rounding = getF(CORNERROUNDING);	// Update input's rounding
	input.GUI = getB(GUI);					// And gui flag,
	input.tick();							// Then check input
	if (frame::mouse::l) {			// if the input is hovered,
		if (input.hovered) {
			frame::selected_component = this;
			frame::selected_type = "TextboxQuark";
		}
		else if (frame::selected_component == this) {
			frame::selected_component = nullptr;
			frame::selected_type = "";
		}
	}
}

void TextboxQuark::keyEvent(int key, int scancode, int action, int mods) {
	if (frame::selected_component != nullptr && frame::selected_type == "TextboxQuark") {
		TextboxQuark* t = static_cast<TextboxQuark*>(frame::selected_component);
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			switch (key) {
			case GLFW_KEY_BACKSPACE:
				if (t->selectStart != t->cursor) {
					t->value.erase((std::min)(t->cursor, t->selectStart), abs((long long) t->cursor - t->selectStart));
					t->cursor = (std::min)(t->cursor, t->selectStart);
					t->selectStart = t->cursor;
				} 
				else if (t->cursor != 0) {
					t->value.erase(t->cursor - 1, 1);
					t->cursor--;
					t->selectStart = t->cursor;
				}
				break;
			case GLFW_KEY_DELETE:
				if (t->selectStart != t->cursor) {
					t->value.erase((std::min)(t->cursor, t->selectStart), abs((long long)t->cursor - t->selectStart));
					t->cursor = (std::min)(t->cursor, t->selectStart);
					t->selectStart = t->cursor;
				}
				else if (t->cursor != t->value.size()) {
					t->value.erase(t->cursor, 1);
				}
				break;

			case GLFW_KEY_LEFT:
				if (t->cursor != 0)
					if (mods & GLFW_MOD_CONTROL)
						for (int i = t->cursor-1; i >= 0; i--) {
							if (i == 0 || ctrlChars.substr(1).find(t->value[i]) != std::string::npos || ctrlChars.find(t->value[i-1]) != std::string::npos) {
								t->cursor = i;
								break;
							}
						}
					else
						t->cursor--;
				if (!(mods & GLFW_MOD_SHIFT))
					t->selectStart=t->cursor;
				break;
			case GLFW_KEY_RIGHT:
				if (t->cursor != t->value.size())
					if (mods & GLFW_MOD_CONTROL && ctrlChars.substr(1).find(t->value[t->cursor]) == std::string::npos)
						for (int i = t->cursor+1; i <= t->value.size(); i++) {
							if (ctrlChars.find(t->value[i]) != std::string::npos || i == t->value.size()) {
								t->cursor = i;
								break;
							}
						}
					else t->cursor++;
				if (!(mods & GLFW_MOD_SHIFT))
					t->selectStart = t->cursor;
				break;

			case GLFW_KEY_UP:
			case GLFW_KEY_HOME:
			case GLFW_KEY_PAGE_UP:
				t->cursor = 0;
				if (!(mods & GLFW_MOD_SHIFT))
					t->selectStart = t->cursor;
				break;

			case GLFW_KEY_DOWN:
			case GLFW_KEY_END:
			case GLFW_KEY_PAGE_DOWN:
				t->cursor = t->value.size();
				if (!(mods & GLFW_MOD_SHIFT))
					t->selectStart = t->cursor;
				break;
			case GLFW_KEY_ENTER:
				frame::selected_component = nullptr;
				break;
			default:
				return;
			}
			t->freezeText(&t->normText);
			t->freezeText(&t->lockedText);
		}
	}
}
void TextboxQuark::charEvent(unsigned int key) {
	if (frame::selected_component != nullptr && frame::selected_type == "TextboxQuark") {
		TextboxQuark* t = static_cast<TextboxQuark*>(frame::selected_component);

		if (t->cursor != t->selectStart) {
			t->value.erase((std::min)(t->cursor, t->selectStart), abs((long long)t->cursor - t->selectStart));
			t->cursor = (std::min)(t->cursor, t->selectStart);
			t->selectStart = t->cursor;
		}

		t->value = t->value.substr(0, t->cursor) + (char32_t)key + t->value.substr(t->cursor, t->value.size());
		t->cursor ++;
		t->selectStart = t->cursor;
		t->freezeText(&t->normText);
		t->freezeText(&t->lockedText);
	}
}

// Render the button
void TextboxQuark::render() {
	if (!frozen) {					// If it's not frozen,
		freezeTextures();			//		freeze it
	}
	set();							// Set the quark variables
	passes::mult_color = glm::vec4(1);	// Make sure colors aren't altered
	passes::add_color = glm::vec4(0);	// Make sure colors aren't altered
	if (input.locked) {
		draw(frozenLocked);
		draw(lockedText);
	}
	else {
		draw(frozenNorm);

		glm::vec2 s = neutron::world::measureString(value.substr(0, cursor), getS(FONT).c_str());
		float W = (s.x / (world::sizeMap[getS(FONT)])) * fontSize;
		float w = W / (world::sizeMap[getS(FONT)] / fontSize);
		s = neutron::world::measureString(value, getS(FONT).c_str());
		W = (s.x / (world::sizeMap[getS(FONT)])) * fontSize;
		float in = s.x * (fontSize / s.y) / 2 - W / 2;				// Then use the measurement to calculate it's offset
		passes::transform = processor::rect(position.x - size.x + in + displayOffset + w + 4, position.y-fontSize/2, 1, fontSize);
		if (sin(glfwGetTime()*2.0 * glm::pi<double>()) > 0 && frame::selected_component == this)
			draw(0, glm::vec4(0, 0, 0, 1));

		set();

		draw(normText);
	}
}

// Freeze a texture
void TextboxQuark::freeze(unsigned int* texture) {
	// Start drawing to the texture
	//processor::drawToTexture2D(*texture, size.x * 2, size.y * 2, GL_LINEAR, GL_RGBA);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glBindTexture(GL_TEXTURE_2D, *texture);  // and then bind it

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x * 2, size.y * 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);	// Set texture to empty as 2d, with RGB and A channels, using unsigned bytes

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		// Use set interpolation close up
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		// ..and far away
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Don't overlap horizontally
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// ..or vertically

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *texture, 0);	// Set a Color attachment to the frame buffer
	glDrawBuffer(GL_COLOR_ATTACHMENT0);										// Set a Color attachment to the draw buffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glViewport(0, 0, size.x * 2, size.y * 2);			// Assign viewport to match the framebuffer size
	glClearColor(0.0, 0.0, 0.0, 0.0);			// Set background color to clear
	glClear(GL_COLOR_BUFFER_BIT);				// Use background color as background

	modes::flip_vertically = !modes::flip_vertically;		// Flip vertically for fix
	if (texture == &frozenLocked) {							// If drawing locked state, use locked colors
		setF4(ADD_COLOR, glm::vec4(0.1, 0.1, 0.1, 0));
		setF4(MULT_COLOR, glm::vec4(0.5, 0.5, 0.5, 1));
		setF4(COLOR, getF4(LOCKEDCOLOR));
	}
	else {													// If drawing default state, use default colors
		setF4(ADD_COLOR, glm::vec4(0));
		setF4(MULT_COLOR, glm::vec4(1));
		setF4(COLOR, getF4(PRIMARYCOLOR));
	}
	passes::transform = processor::form(glm::vec3(1, -1, 0) * size, this->size);	// Set transformation, translating up for fixed vertical flip
	passes::add_color = getF4(ADD_COLOR);	// Set add
	passes::mult_color = getF4(MULT_COLOR);	// And mult color

	passes::cust_float4s["bounds"] = glm::vec4(size, 1);	// Set shader's bounds variable size
	passes::cust_floats["rounding"] = getF(CORNERROUNDING);	// Set the shader's rounding
	passes::cust_floats["bevel"] = getF(BEVEL);				// Set shader's bevel

	if (lookType == FLAT) {				// Draw flat
		passes::shader = passes::SHADER_BUTTON;
		draw(0, getF4(COLOR), layer, 6);
	}
	else if (lookType == JARVOPIA) {	// Draw JARvopia
		passes::shader = passes::SHADER_BUTTON;
		passes::mult_color = getF4(MULT_COLOR) * glm::vec4(2, 2, 2, 1); // Correct for darker texture
		draw(processor::getTextureFrom("res/textures/JARvopiaButton.png", GL_NEAREST), getF4(COLOR), layer, 6);
	}
	else if (lookType == OUTLINE) {		// Draw only outline
		passes::shader = neutron::processor::LoadShader("res/shaders/buttonOutline.shader");
		draw(0, getF4(COLOR), layer, 6);
	}

	modes::flip_vertically = !modes::flip_vertically;	// Set vertical flip back
	passes::transform = processor::rect(0, 0, size.x * 2, size.y * 2);
	passes::shader = passes::SHADER_FLAT;
	if(texture == &normText)
		draw(0, glm::vec4(0, 1, 0, 1));

	// Select text colors
	if (texture == &frozenLocked)
		setF4(COLOR, getF4(INVERTEDTEXTCOLOR));
	else
		setF4(COLOR, getF4(PRIMARYTEXTCOLOR));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);				// Unbind texture, set new draw path to screen
	glViewport(0, 0, frame::width, frame::height);
}

void TextboxQuark::freezeText(unsigned int *text) {
	// Start drawing to the texture
	//processor::drawToTexture2D(*texture, size.x * 2, size.y * 2, GL_LINEAR, GL_RGBA);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);;

	glBindTexture(GL_TEXTURE_2D, *text);  // and then bind it

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x * 2, size.y * 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);	// Set texture to empty as 2d, with RGB and A channels, using unsigned bytes

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		// Use set interpolation close up
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		// ..and far away
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Don't overlap horizontally
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// ..or vertically

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *text, 0);	// Set a Color attachment to the frame buffer
	glDrawBuffer(GL_COLOR_ATTACHMENT0);										// Set a Color attachment to the draw buffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glViewport(0, 0, size.x * 2, size.y * 2);			// Assign viewport to match the framebuffer size
	glClearColor(0.0, 0.0, 0.0, 0.0);					// Set background color to clear
	glClear(GL_COLOR_BUFFER_BIT);						// Use background color as background

	modes::flip_vertically = !modes::flip_vertically;		// Flip vertically for fix
	if (text == &lockedText) {								// If drawing locked state, use locked colors
		setF4(ADD_COLOR, glm::vec4(0.1, 0.1, 0.1, 0));
		setF4(MULT_COLOR, glm::vec4(0.5, 0.5, 0.5, 1));
		setF4(COLOR, getF4(INVERTEDTEXTCOLOR));
	}
	else {													// If drawing default state, use default colors
		setF4(ADD_COLOR, glm::vec4(0));
		setF4(MULT_COLOR, glm::vec4(1));
		setF4(COLOR, getF4(PRIMARYTEXTCOLOR));
	}

	passes::add_color = getF4(ADD_COLOR);	// Set add
	passes::mult_color = getF4(MULT_COLOR);	// And mult color
	modes::gui = true;						/// as a gui :: CAN BE REPLACED LATER WHEN USING PARAGRAPHS IN THE 3D SPACE
	modes::alpha_test = false;				// And don't alpha-test, so that opacity in each pixel is preserved
	passes::shader = passes::SHADER_FLAT;	// Use default shader
	modes::draw = modes::DRAW_GLYPH;

	float uni = 4 + displayOffset;
	float len1 = 0;
	float len2 = 0;
	modes::text_equispace = false;
	glm::vec2 s = neutron::world::measureString(value.substr(0, cursor), getS(FONT).c_str());
	float W = ((s.x / (world::sizeMap[getS(FONT)])) * fontSize) / (world::sizeMap[getS(FONT)] / fontSize);
	if (-W > uni-4) {
		displayOffset = -W;
		uni = 4 + displayOffset;
	}
	if (W + uni > size.x * 2 - 10) {
		displayOffset = -(10 + W - (size.x * 2));
		uni = 4 + displayOffset;
	}
	s = neutron::world::measureString(value, getS(FONT).c_str());
	W = (s.x / (world::sizeMap[getS(FONT)])) * fontSize;
	len1 = W / (world::sizeMap[getS(FONT)] / fontSize);
	uni += s.x * (fontSize / s.y) / 2 - W / 2;				// Then use the measurement to calculate it's offset
	s = neutron::world::measureString(value.substr((std::min)(selectStart, cursor), abs(selectStart-(long long)cursor)), getS(FONT).c_str());
	len2 = ((s.x / (world::sizeMap[getS(FONT)])) * fontSize) / (world::sizeMap[getS(FONT)] / fontSize);
	
	passes::transform = processor::rect(uni, -(fontSize/1.75+size.y), len1, fontSize);
	drawText(value, getS(FONT).c_str(), 0, getF4(PRIMARYTEXTCOLOR), 0.0, 6);
	s = neutron::world::measureString(value.substr(0, (std::min)(selectStart, cursor)), getS(FONT).c_str());
	len1 = ((s.x / (world::sizeMap[getS(FONT)])) * fontSize) / (world::sizeMap[getS(FONT)] / fontSize);

	passes::transform = processor::rect(uni + len1, -(fontSize / 2 + size.y), len2, fontSize);
	modes::draw = modes::DRAW_FLAT;
	draw(0, glm::vec4(0, 0.6, 1, 0.5));
	passes::glyph_clip = 0.3;
	modes::draw = modes::DRAW_GLYPH;
	passes::transform = processor::rect(uni + len1, -(fontSize / 1.75 + size.y), len2, fontSize);
	drawText(value.substr((std::min)(selectStart, cursor), abs(selectStart - (long long)cursor)), getS(FONT).c_str(), 0, getF4(INVERTEDTEXTCOLOR), 0.0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);				// Unbind texture, set new draw path to screen
	modes::flip_vertically = !modes::flip_vertically;	// Set vertical flip back
	glViewport(0, 0, frame::width, frame::height);
}