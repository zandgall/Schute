#pragma once
#include "Envelope.h"
#include "Neutron/Quark.h"
#include "Neutron/ButtonQuark.h"
#include "Neutron/TabQuark.h"
#include "Neutron/SliderQuark.h"
#include "Neutron/TextboxQuark.h"
#include "World.h"
#include "FileIO/ListTag.h";
#include "FileIO/CompoundTag.h"
#include <vector>
#include <map>
typedef ButtonQuark Button;
typedef SliderQuark Slider;
typedef TextboxQuark Textbox;
typedef TabQuark Menu;
class LevelMakerEnvelope :
	public Envelope
{
public:
	float xOff = 0, yOff = 0, xOffVel = 0, yOffVel = 0, scroll = 10.0;

	int tileX = 0, tileY = 0, ptX = 0, ptY = 0;

	static glm::ivec2 spawn;

	static int width, height;
	int tile = 1, selectedEnt = -1;

	unsigned int coinTexture = 0, enemyTexture = 0, playerTexture = 0, playerHead = 0, frame1 = 0, frame2 = 0, door0 = 0, door1 = 1;
	unsigned int button [8];

	//Button addBG = Button();
	//Slider layer;

	Button ent_del;

	MouseDetector entMove;

	static World world;

	static ListTag entities;

	Menu menu;

	std::vector<glm::ivec2> coins = std::vector<glm::ivec2>(), enemies = std::vector<glm::ivec2>();

	std::map<std::string, std::map<std::string, Button>> ent_B = std::map<std::string, std::map<std::string, Button>>();
	std::map<std::string, std::map<std::string, Slider>> ent_S = std::map<std::string, std::map<std::string, Slider>>();
	std::map<std::string, std::map<std::string, Textbox>> ent_E = std::map<std::string, std::map<std::string, Textbox>>();
	std::map<int, MouseDetector> ent_Interactive = std::map<int, MouseDetector>();	
	std::map<std::string, CompoundTag> ent_T = std::map<std::string, CompoundTag>();

	LevelMakerEnvelope();
	~LevelMakerEnvelope();
	void tick();
	void render();
	void render_gui();
private:
	static void resizeworld();

	static void saveImage();
	static void saveWorld();
	static void openWorld();

	static void wpp();
	static void wmm();
	static void hpp();
	static void hmm();
	
	CompoundTag* add(std::string type) {
		CompoundTag *a = new CompoundTag("");
		(*a) << new StringTag("type", type);
		(*a) << new IntTag("x", tileX);
		(*a) << new IntTag("y", tileY);
		return a;
	}
};

