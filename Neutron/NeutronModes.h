#pragma once
#ifndef NEUTRON_MODES
#define NEUTRON_MODES
#include <vector>
#include <glm/glm.hpp>
namespace neutron {
	// Different options for how Neutron should handle things
	namespace modes {
		// DRAW_FLAT just draws the texture and/or color as is. DRAW_GLYPH draws using a distance texture to map out a glyph. DRAW_INSTANCED uses instance methods with certain functions like drawText
		const unsigned int DRAW_FLAT = 0, DRAW_GLYPH = 1, DRAW_INSTANCED = 2;
		// draw = changes how draw() should handle it's parameters : draw_def = what draw will be on reset
		extern unsigned int draw, draw_def;

		// Draw in a GUI mode or not, removing perspective from what you're going to draw
		extern bool gui, gui_def;

		// Test alpha whether to discard under values of 0.8 or to rely on sorted blending. Disabling works mostly with gui : def is default on neutron::reset();
		extern bool alpha_test, alpha_test_def;

		// Whether to anti-alias or not
		extern bool anti_alias, anti_alias_def;

		// Different flipping options
		extern bool flip_horizontally, flip_vertically;

		// Text options
		extern bool text_equispace, text_equispace_def;

		// Set everything to the default values after drawing?
		extern bool reset_on_draw;

		// Whether or not Neutron should post process
		extern bool post_processing;
		namespace post {
			// The Framebuffer that will be modified and pushed to the screen when done rendering
			extern unsigned int screen;
			// The Texture assigned with the screen Framebuffer
			extern unsigned int screenTex;
			// A type of post processing
			extern float vignette, bloom, bloomIterations, bloomThreshold, gamma;
			// Post processing variable
			extern glm::vec4 vignetteColor, hsvAdd, hsvMult, rgbAdd, rgbMult;
			// Color selection and correction, where the first layer is the color you'd want to change, the second layer is the tolerance of the color selection, the third layer is the value you want to multiply by, and the fourth layer is the layer to add by
			extern std::vector<glm::mat4x3> hsvCorrection, rgbCorrection;

			extern void set();
			extern void resetScreenSize(int width, int height);
			extern void bindPost();
			extern void pushToScreen();
		};

		extern void reset();
		extern void set();
	};
};
#endif