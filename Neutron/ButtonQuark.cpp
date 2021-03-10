#include "ButtonQuark.h"
#include "NeutronPasses.h"
#include "NeutronFrame.h"
#include "NeutronProcessor.h"
#include "NeutronModes.h"
using namespace neutron;
// Update the button and check for input
void ButtonQuark::tick() {
	input.rounding = getF(CORNERROUNDING);	// Update input's rounding
	input.GUI = getB(GUI);					// And gui flag,
	input.tick();							// Then check input
	if (input.hovered) {
		frame::selected_component = this;
		frame::selected_type = "SliderQuark";
	}
	else if (frame::selected_component == this) {
		frame::selected_component = nullptr;
		frame::selected_type = "";
	}
	if (interactionType == TOGGLE) {		// if the interaction type is toggle,
		if (input.lClicked)					// if there's input,
			on = !on;						// invert on
	}
	else
		on = input.lClicked;				// Otherwise set on equal to the given input
	displayOn = on ? 10 : displayOn;		// Set a display delay, so the 'on' state renders a few more times
	if (input.lClicked && onClicked != nullptr)	// If there is input, and a function to run when clicked,
		onClicked();							// run the "onClicked" function
}
// Render the button
void ButtonQuark::render() {
	if (!frozen) {					// If it's not frozen,
		freezeTextures();			//		freeze it
	}
	set();							// Set the quark variables
	passes::mult_color = glm::vec4(1);	// Make sure colors aren't altered
	passes::add_color = glm::vec4(0);	// Make sure colors aren't altered
	if (input.locked)
		draw(frozenLocked);
	else if (on && input.hovered)	// Renders when in toggle state, and button is hovered while it's toggled
		draw(frozenToggled);
	else if (displayOn)
		draw(frozenOn);
	else if (input.hovered)
		draw(frozenHovered);
	else
		draw(frozenNorm);
	if (displayOn)
		displayOn--;				// Reduce the display on delay every loop
}

// Freeze a texture
void ButtonQuark::freeze(unsigned int *texture) {
	// Start drawing to the texture
	//processor::drawToTexture2D(*texture, size.x * 2, size.y * 2, GL_LINEAR, GL_RGBA);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glBindTexture(GL_TEXTURE_2D, *texture);  // and then bind it

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x*2, size.y*2, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);	// Set texture to empty as 2d, with RGB and A channels, using unsigned bytes

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
	else if (texture == &frozenToggled) {					// If drawing toggled state, use toggled colors
		setF4(ADD_COLOR, glm::vec4(-0.2, -0.2, -0.2, 0));
		setF4(MULT_COLOR, glm::vec4(0.9, 0.9, 0.9, 1));
		setF4(COLOR, getF4(HOVEREDONCOLOR));
	}
	else if (texture == &frozenOn) {						// If drawing on state, use on colors
		setF4(ADD_COLOR, glm::vec4(-0.05, -0.05, -0.05, 0));
		setF4(MULT_COLOR, glm::vec4(0.5, 0.5, 0.5, 1));
		setF4(COLOR, getF4(ONCOLOR));
	}
	else if (texture == &frozenHovered) {					// If drawing hovered state, use hovered colors
		setF4(ADD_COLOR, glm::vec4(0.075, 0.075, 0.075, 0));
		setF4(MULT_COLOR, glm::vec4(1));
		setF4(COLOR, getF4(HOVEREDCOLOR));
	}
	else {													// If drawing default state, use default colors
		setF4(ADD_COLOR, glm::vec4(0));
		setF4(MULT_COLOR, glm::vec4(1));
		setF4(COLOR, getF4(PRIMARYCOLOR));
	}
	passes::transform = processor::form(glm::vec3(1, -1, 0)*size, this->size);	// Set transformation, translating up for fixed vertical flip
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
		passes::mult_color = getF4(MULT_COLOR)*glm::vec4(2, 2, 2, 1); // Correct for darker texture
		draw(processor::getTextureFrom("res/textures/JARvopiaButton.png", GL_NEAREST), getF4(COLOR), layer, 6);
	}
	else if (lookType == OUTLINE) {		// Draw only outline
		passes::shader = neutron::processor::LoadShader("res/shaders/buttonOutline.shader");
		draw(0, getF4(COLOR), layer, 6);
	}

	// Select text colors
	if (texture == &frozenLocked)
		setF4(COLOR, getF4(LOCKEDTEXTCOLOR));
	else if (texture == &frozenToggled)
		setF4(COLOR, getF4(HOVEREDONTEXTCOLOR));
	else if (texture == &frozenOn)
		setF4(COLOR, getF4(ONTEXTCOLOR));
	else if (texture == &frozenHovered)
		setF4(COLOR, getF4(HOVEREDTEXTCOLOR));
	else
		setF4(COLOR, getF4(PRIMARYTEXTCOLOR));

	// Draw flat, as a glyph, at a different height
	passes::shader = passes::SHADER_FLAT;
	modes::draw = modes::DRAW_GLYPH;
	passes::transform = processor::form(glm::vec3(1, -1.25, 0) * size, size * glm::vec3(0.75));
	drawText(getS(TEXT), getS(FONT).c_str(), 0, getF4(COLOR), layer, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);				// Unbind texture, set new draw path to screen
	modes::flip_vertically = !modes::flip_vertically;	// Set vertical flip back
	glViewport(0, 0, frame::width, frame::height);
}