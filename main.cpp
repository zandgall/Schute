#include "main.h"
#include "GameEnvelope.h"
#include "LevelMakerEnvelope.h"
#include "NBTEnvelope.h"
#include "FileIO/FileChooser.h"
#include "Neutron/SerialImage.h"
#include "Neutron/VectorImage.h"
#include "Neutron/Neutron.h"
#include "Neutron/NeutronProcessor.h"
#include "Neutron/NeutronWorld.h"
#include "Neutron/NeutronFrame.h"
#include "Neutron/NeutronModes.h"
#include "Neutron/Quark.h"
#include "Neutron/ButtonQuark.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Neutron/stb_image.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <Windows.h>

using namespace neutron;
using namespace neutron::processor;

int main() {
	// Start neutron and set OpenGL configs
	neutron::set();
	
	glDisable(GL_CULL_FACE);

	//Draw lines
	world::loadFont("res/fonts/Arial.ttf", "arial", 48);

	modes::post_processing = false;

	UsedEnvelope = new GameEnvelope();
	//UsedEnvelope = new LevelMakerEnvelope();
	//UsedEnvelope = new NBTEnvelope(); 

	while (frame::running()) {

		neutron::beforeLoop();

		render();

		neutron::afterLoop();
	}

	// terminate
	glfwTerminate();
	return 0;
}

void render() {
	// render
	glViewport(0, 0, frame::width, frame::height);
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f); // Set the clear color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear
	
	glEnable(GL_DEPTH_TEST);
	// Render 3d
	UsedEnvelope->tick();
	UsedEnvelope->render();

	glDisable(GL_DEPTH_TEST);
	// Render gui
	UsedEnvelope->render_gui();
}