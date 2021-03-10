#pragma once
#ifndef NEUTRON_FRAME
#define NEUTRON_FRAME
#include <vector>
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
namespace neutron {
	// To run and start a window and get input
	namespace frame {

		// Current window as set in GLFW
		extern GLFWwindow* window;
		// Width and height of window
		extern int width, height;

		// Time from this frame to last frame, use for stable movement
		extern double delta_time;
		// Time at last frame
		extern double last_time;
		// Time at this frame
		extern double this_time;
		// Duration of last frame
		extern double duration;

		// Current FPS
		extern float fps;
		// Estimated FPS
		extern double estimate_fps;

		// Current selected object of the screen
		extern void* selected_component;

		extern std::string selected_type;

		extern void pause();
		extern void unpause();

		extern bool running();

		extern int set(std::string name = "Neutron", int width = 800, int height = 800);

		extern void checkInput();

		extern void swap();

		extern void beginFrame();

		extern void stop();

		namespace mouse {

			// Mouse Button inputs
			extern bool left, right, middle, l, r, m, lClicked, rClicked, mClicked, lReleased, rReleased, mReleased;
			// Mouse Position inputs
			extern float x, y, pX, pY, prevX, prevY, nextX, nextY, nX, nY;
			// Mouse Scroll inputs
			extern double totalScroll, scroll, x_scroll, y_scroll, x_s, y_s, scroll_glide, x_scroll_glide, y_scroll_glide, scroll_g, x_s_g, y_s_g, GLIDE_DECAY;

			extern int initWithWindow(GLFWwindow* window);

			extern void lock();
			extern void unlock();
			extern void endFrame();
			extern bool isLocked();
		};

		namespace keys {

			extern std::vector<bool> keys;
			extern bool typed, released, recordTyping;
			extern std::string typedInputString;
			extern std::map<unsigned int, void(*)()> pressedEvents, releasedEvents, typedEvents;
			extern std::vector<void(*)(int key, int scancode, int action, int mods)> keyEvents;
			extern std::vector<void(*)(unsigned int key)> charEvents;

			extern int initWithWindow(GLFWwindow* window);
			extern void beginFrame();
			extern void endFrame();
			extern void whenKeyPressedDo(unsigned int keyId, void(*function)());
			extern void whenKeyTypedDo(unsigned int keyId, void(*function)());
			extern void whenKeyReleasedDo(unsigned int keyId, void(*function)());
			
			extern void addKeyEvent(void(*function)(int key, int scancode, int action, int mods));
			extern void addCharEvent(void(*function)(unsigned int key));

		};
	};
};
#endif