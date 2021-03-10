#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "LevelMakerEnvelope.h"
#include "Neutron/NeutronProcessor.h"
#include "Neutron/Neutron.h"
#include "Neutron/NeutronPasses.h"
#include "Neutron/NeutronModes.h"
#include "Neutron/NeutronFrame.h"
#include "Neutron/Quark.h"
#include "Neutron/ParagraphQuark.h"
#include "Neutron/SerialImage.h"
#include "FileIO/Tag.h"
#include "FileIO/CompoundTag.h"
#include "FileIO/IntTag.h"
#include "FileIO/IntArrayTag.h"
#include "FileIO/ByteArrayTag.h"
#include "FileIO/FileChooser.h"
#include "Neutron/stb_image.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Neutron/stb_image_write.h"
#include <iostream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>
#include <codecvt>
#include <vector>
#include <numeric>
#include <Windows.h>

// Source: https://stackoverflow.com/a/46711735
// Copied from: https://gcc.godbolt.org/z/pfJXRm
constexpr inline long long int hash(char const* str, int h = 0) {
	return (!str[h] ? 5381 : (hash(str, h + 1) * 33) ^ str[h]);
}

constexpr inline long long int operator "" _(char const* p, size_t) { return hash(p); }

inline long long int hash(std::string const& s) { return hash(s.c_str()); }
// End of string 'hashing'

using namespace neutron;

std::map<float, std::string> bgs = std::map<float, std::string>();
float LVLMKR_layer = 0;

World LevelMakerEnvelope::world = World();
int LevelMakerEnvelope::width = 10;
int LevelMakerEnvelope::height = 5;
glm::ivec2 LevelMakerEnvelope::spawn = glm::ivec2(2);
ListTag LevelMakerEnvelope::entities = ListTag("entities", Tag::COMPOUND);;

void loadBG() {
	bgs.emplace(LVLMKR_layer, ChooseOpenFile("Pick background image", "PNG image files (*.png)\0*.png\0Jpeg Image files (*.jpg)\0*.jpg\0Image Files (*.bmp;*.jpg;*.png;*.tga)\0*.bmp;*.jpg;*.png;*.tga\0All files (*.*)\0*.*").c_str());
}

LevelMakerEnvelope::LevelMakerEnvelope() {
	world.set(10, 5);
	world.updateTexture();
	coinTexture = neutron::processor::getCroppedTexture(neutron::processor::getTextureFrom("res/textures/Coin.png", GL_NEAREST), 0, 0, 16, 16);
	playerHead = neutron::processor::getCroppedTexture(neutron::processor::getTextureFrom("res/textures/Player.png", GL_NEAREST), 0, 0, 16, 16);
	playerTexture = neutron::processor::getCroppedTexture(neutron::processor::getTextureFrom("res/textures/Player.png", GL_NEAREST), 32, 0, 16, 16);
	enemyTexture = neutron::processor::getCroppedTexture(neutron::processor::getTextureFrom("res/textures/Enemy.png", GL_NEAREST), 0, 16, 16, 16);
	door0 = neutron::processor::getCroppedTexture(neutron::processor::getTextureFrom("res/textures/door.png", GL_NEAREST, GL_REPEAT), 0, 0, 16, 16);
	door1 = neutron::processor::getCroppedTexture(neutron::processor::getTextureFrom("res/textures/door.png", GL_NEAREST, GL_REPEAT), 16, 0, 16, 16);
	
	ent_del = Button(glm::vec3(10, 60, 0), glm::vec3(100, 20, 0));
	ent_del.lookType = Button::OUTLINE;
	ent_del.setS(Button::TEXT, "Delete");
	ent_del.setColorPallete(glm::vec4(1, 0, 0, 1));
	ent_del.setTextPallete(glm::vec4(1, 0, 0, 1));
	ent_del.setF(Button::CORNERROUNDING, 10.f);
	ent_del.setF(Button::BEVEL, 0.2f);

	world = World();
	
	unsigned int buttonTEX = neutron::processor::getTextureFrom("res/textures/button.png", GL_NEAREST, GL_CLAMP_TO_EDGE);
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			button[i * 4 + j] = neutron::processor::getCroppedTexture(buttonTEX, j*16, i*8, 16, 8);
		}
	}

	menu = Menu(glm::vec3(-408, 705, 0), glm::vec3(816, 70, 0), glm::vec3(800, 70, 0), "Main", 60, 40);
	
	//menu.setF(Menu::BEVEL, 0.1);
	Button* saveButton = new Button(glm::vec3(10, 10, 0), glm::vec3(100, 40, 0));
	saveButton->setS(Quark::TEXT, "Save");
	saveButton->onClickedDo(&saveWorld);
	menu[0]->put("save", saveButton);
	Button* loadButton = new Button(glm::vec3(120, 10, 0), glm::vec3(100, 40, 0));
	loadButton->setS(Quark::TEXT, "Load");
	loadButton->onClickedDo(&openWorld);
	menu[0]->put("load", loadButton);
	Button* imgButton = new Button(glm::vec3(230, 10, 0), glm::vec3(150, 40, 0));
	imgButton->setS(Quark::TEXT, "Save Base");
	imgButton->onClickedDo(&saveImage);
	menu[0]->put("saveImage", imgButton);

	Button* wmm = new Button(glm::vec3(400, 10, 0), glm::vec3(40, 40, 0));
	wmm->setS(Quark::TEXT, "-");
	wmm->onClickedDo(&LevelMakerEnvelope::wmm);
	menu[0]->put("wmm", wmm);
	Button* wpp = new Button(glm::vec3(550, 10, 0), glm::vec3(40, 40, 0));
	wpp->setS(Quark::TEXT, "+");
	wpp->onClickedDo(&LevelMakerEnvelope::wpp);
	menu[0]->put("wpp", wpp);

	Button* widthLabel = new Button(glm::vec3(440, 10, 0), glm::vec3(110, 40, 0));
	widthLabel->setS(Quark::TEXT, "Width: 10");
	widthLabel->lookType = Button::TEXTONLY;
	menu[0]->put("widthLabel", widthLabel);

	Button* hmm = new Button(glm::vec3(600, 10, 0), glm::vec3(40, 40, 0));
	hmm->setS(Quark::TEXT, "-");
	hmm->onClickedDo(&LevelMakerEnvelope::hmm);
	menu[0]->put("hmm", hmm);

	Button* hpp = new Button(glm::vec3(750, 10, 0), glm::vec3(40, 40, 0));
	hpp->setS(Quark::TEXT, "+");
	hpp->onClickedDo(&LevelMakerEnvelope::hpp);
	menu[0]->put("hpp", hpp);

	Button* heightLabel = new Button(glm::vec3(640, 10, 0), glm::vec3(110, 40, 0));
	heightLabel->setS(Quark::TEXT, "Height: 5");
	heightLabel->lookType = Button::TEXTONLY;
	menu[0]->put("heightLabel", heightLabel);

	menu[0]->put("split1", new PageQuark::Splitter(30, 20, 1, 395));
	menu[0]->put("split2", new PageQuark::Splitter(30, 20, 1, 595));
	menu[0]->put("split2", new PageQuark::Splitter(30, 20, 1, 795));

	/// PAGE 2
	menu.put("Tiles", 70);
	menu[1]->put("tile", new Quark(glm::vec3(30, 30, 0), 0.0, glm::vec3(20, 20, 0)));
	(*menu[1])["tile"]->setB(Quark::GUI, true);
	frame1 = processor::getCroppedTexture(processor::getTextureFrom("res/textures/Tiles.png"), 48, 48, 16, 16);
	frame2 = processor::getCroppedTexture(processor::getTextureFrom("res/textures/Tiles2.png"), 48, 48, 16, 16);
	(*menu[1])["tile"]->setUINT(Quark::TEXTURE, frame1);


	///  PAGE 3
	menu.put("Entities", 100);
	
	menu[2]->put("coin", new Quark(glm::vec3(30, 30, 0), 0.f, glm::vec3(20, 20, 0.0)));
	(*menu[2])["coin"]->setB(Quark::GUI, true);
	(*menu[2])["coin"]->setUINT(Quark::TEXTURE, processor::getCroppedTexture(processor::getTextureFrom("res/textures/Coin.png"), 0, 0, 16, 16));
	
	menu[2]->put("rock", new Quark(glm::vec3(70, 30, 0), 0.f, glm::vec3(20, 20, 0.0)));
	(*menu[2])["rock"]->setB(Quark::GUI, true);
	(*menu[2])["rock"]->setUINT(Quark::TEXTURE, processor::getTextureFrom("res/textures/Rock.png"));
	
	menu[2]->put("button", new Quark(glm::vec3(110, 30, 0), 0.f, glm::vec3(20, 20, 0.0)));
	(*menu[2])["button"]->setB(Quark::GUI, true);
	(*menu[2])["button"]->setUINT(Quark::TEXTURE, processor::getCroppedTexture(processor::getTextureFrom("res/textures/button.png"), 48, 0, 16, 16));
	
	menu[2]->put("door", new Quark(glm::vec3(150, 30, 0), 0.f, glm::vec3(20, 20, 0.0)));
	(*menu[2])["door"]->setB(Quark::GUI, true);
	(*menu[2])["door"]->setUINT(Quark::TEXTURE, processor::getCroppedTexture(processor::getTextureFrom("res/textures/door.png", GL_NEAREST, GL_REPEAT), 0, 0, 16, 16));
	
	ent_T.insert(std::make_pair("coin", CompoundTag()));
	ent_T.insert(std::make_pair("rock", CompoundTag()));
	ent_T.insert(std::make_pair("button", CompoundTag()));
	ent_T["button"] << new IntTag("size", 3);
	ent_T["button"] << new DoubleTag("size~~min", 1);
	ent_T["button"] << new DoubleTag("size~~max", 6);
	ent_T["button"] << new DoubleTag("size~~def", 3);
	ent_T["button"] << new ByteTag("toggle", '\0');
	ent_T["button"] << new StringTag("event", "event id");
	ent_T.insert(std::make_pair("door", CompoundTag()));
	ent_T["door"] << new IntTag("size", 3);
	ent_T["door"] << new DoubleTag("size~~min", 1);
	ent_T["door"] << new DoubleTag("size~~max", 10);
	ent_T["door"] << new DoubleTag("size~~def", 3);
	ent_T["door"] << new ByteTag("toggle", '\0');
	ent_T["door"] << new ByteTag("horizontal", '\0');
	ent_T["door"] << new StringTag("event", "event id");

	//ent_B.insert(std::make_pair("coin", std::map<std::string, Button>()));
	//ent_S.insert(std::make_pair("coin", std::map<std::string, Slider>()));
	//ent_E.insert(std::make_pair("coin", std::map<std::string, Textbox>()));

	//ent_B.insert(std::make_pair("rock", std::map<std::string, Button>()));
	//ent_S.insert(std::make_pair("rock", std::map<std::string, Slider>()));
	//ent_E.insert(std::make_pair("rock", std::map<std::string, Textbox>()));

	//ent_B.insert(std::make_pair("button", std::map<std::string, Button>()));
	//ent_S.insert(std::make_pair("button", std::map<std::string, Slider>()));
	//ent_E.insert(std::make_pair("button", std::map<std::string, Textbox>()));

	//ent_S["button"].insert(std::make_pair("size", Slider(glm::vec3(10, 10, 0), glm::vec3(100, 40, 0), 1, 6, 3)));
	//ent_B["button"].insert(std::make_pair("toggle", Button(glm::vec3(120, 10, 0), glm::vec3(100, 40, 0))));
	//ent_B["button"]["toggle"].interactionType = Button::TOGGLE;
	//ent_B["button"]["toggle"].setS(Button::TEXT, "Toggle");
	//ent_E["button"].insert(std::make_pair("event", Textbox(glm::vec3(230, 10, 0), glm::vec3(100, 40, 0), U"Text", 32.f)));
	for (auto i = ent_T.begin(); i != ent_T.end(); i++) {
		ent_B.insert(std::make_pair(i->first, std::map<std::string, Button>()));
		ent_S.insert(std::make_pair(i->first, std::map<std::string, Slider>()));
		ent_E.insert(std::make_pair(i->first, std::map<std::string, Textbox>()));

		int nX = 10;
		for (auto j = i->second().begin(); j != i->second().end(); j++) {
			std::u32string n = U"";
			std::string name = "";
			switch ((*j)->id()) {
			case Tag::BYTE:
				ent_B[i->first].insert(std::make_pair((*j)->getName(), Button(glm::vec3(nX, 10, 0), glm::vec3(100, 40, 0))));
				ent_B[i->first][(*j)->getName()].interactionType = Button::TOGGLE;
				name = (*j)->getName();
				name[0] = std::toupper(name[0]);
				ent_B[i->first][(*j)->getName()].setS(Button::TEXT, name);
				nX += 110;
				break;
			case Tag::STRING:
				n = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes((*j)->getName());
				n[0] = std::toupper(n[0]);
				ent_E[i->first].insert(std::make_pair((*j)->getName(), Textbox(glm::vec3(nX, 10, 0), glm::vec3(100, 40, 0), n, 32.f)));
				nX += 110;
				break;
			default:
				if (std::string((*j)->getName()).find("~~min") != std::string::npos || std::string((*j)->getName()).find("~~max") != std::string::npos || std::string((*j)->getName()).find("~~def") != std::string::npos)
					break;
				ent_S[i->first].insert(std::make_pair((*j)->getName(), Slider(glm::vec3(nX, 10, 0), glm::vec3(100, 40, 0), i->second.d(std::string((*j)->getName()) + "~~min")(), i->second.d(std::string((*j)->getName()) + "~~max")(), i->second.d(std::string((*j)->getName()) + "~~def")())));
				nX += 110;
			}
		}
	}
}


LevelMakerEnvelope::~LevelMakerEnvelope() {

}

void LevelMakerEnvelope::resizeworld() {
	unsigned int old_width = world.width + 0, old_height = world.height + 0;
	std::vector<unsigned char> old_tiles = world.tiles;

	world.set(width, height);
	for (int x = 0; x < old_width && x < width; x++) {
		for (int y = 0; y < old_height && y < height; y++) {
			if (x < width && x < old_width && y < old_height && y < height)
				world.tiles[y*width + x] = old_tiles[y*old_width + x];
		}
	}
	world.updateTexture();
}

void s(int x, int y, char v, World *w) {
	if (!(x >= 0 && x < w->width&&y >= 0 && y < w->height))
		return;
	w->tiles[x + y * w->width] = v;
}

void LevelMakerEnvelope::saveImage() {
	world.updateTexture();
	glBindTexture(GL_TEXTURE_2D, world.texture);
	unsigned char* imgdat = (unsigned char*)malloc((int)(world.width * World::tileSize * world.height * World::tileSize * 4));
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgdat);
	std::string output = garunteeExtension(ChooseSaveFile("Save Temporary World Image", "PNG image files (*.png)\0*.png"), ".png");
	stbi_flip_vertically_on_write(true);
	stbi_write_png(output.c_str(), world.width * World::tileSize, world.height * World::tileSize, 4, imgdat, world.width * World::tileSize * 4);
	delete[] imgdat;
}

void LevelMakerEnvelope::saveWorld() {
	std::cout << "Writing basic data..." << std::endl;
	CompoundTag t = CompoundTag();
	t << new IntTag("width", width);
	t << new IntTag("height", height);

	t << new IntTag("spawnX", spawn.x);
	t << new IntTag("spawnY", spawn.y);

	std::vector<unsigned char> tiles = std::vector<unsigned char>();
	for (int i = 0; i < width * height; i++)
		tiles.push_back(world.tiles[i]);
	t << new ByteArrayTag("tiles", tiles);

	t << &entities;
	t << new ByteArrayTag("baseImg", readAsBytes(ChooseOpenFile("Pick base image", "PNG image files (*.png)\0*.png\0Jpeg Image files (*.jpg)\0*.jpg\0Image Files (*.bmp;*.jpg;*.png;*.tga)\0*.bmp;*.jpg;*.png;*.tga\0All files (*.*)\0*.*").c_str()));
	ListTag bg = ListTag("bg", Tag::COMPOUND);
	std::cout << "Encoding background images..." << std::endl;
	for (std::map<float, std::string>::iterator i = bgs.begin(); i != bgs.end(); i++) {
		CompoundTag v = CompoundTag("");
		v << new FloatTag("layer", i->first);
		v << new ByteArrayTag("img", readAsBytes(i->second.c_str()));
		bg.addValue(&v);
	}
	t << &bg;

	std::cout << "Saving world..." << std::endl;
	std::vector<unsigned char> outbytes = t.outBytes();
	writeBinary(ChooseSaveFile("Save World", "Binary files (*.bin)\0*.bin\0All files (*.*)\0*.*").c_str(), &outbytes[0], outbytes.size());
	t.getCompilation();
	t.~CompoundTag();
	outbytes.clear();
}

void LevelMakerEnvelope::openWorld() {
	world = World();
	CompoundTag t = loadNBT(ChooseOpenFile("Pick world", "Binary World Files (*.bin)\0*.bin\0All Files (*.*)\0*.*").c_str());
	width = t.i("width")();
	height = t.i("height")();
	world.set(width, height);

	spawn = glm::ivec2(t.i("spawnX")(), t.i("spawnY")());

	std::vector<unsigned char> tiles = t.B("tiles")();
	for (auto i = 0; i < tiles.size(); i++)
		world.tiles[i] = tiles[i];
	tiles.clear();

	entities = t.li("entities");

	int xx, yy, com;
	std::vector<unsigned char> imgArray = t.B("baseImg")();
	stbi_set_flip_vertically_on_load(true);
	unsigned char* img = stbi_load_from_memory(&imgArray[0], imgArray.size(), &xx, &yy, &com, 4);
	glBindTexture(GL_TEXTURE_2D, world.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xx, yy, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	stbi_image_free(img);
	t.getCompilation();
}

void LevelMakerEnvelope::wpp() {
	float mult = frame::keys::keys[GLFW_KEY_LEFT_CONTROL] ? 10 : frame::keys::keys[GLFW_KEY_LEFT_SHIFT] ? 5 : 1;
	width += mult;
	resizeworld();
}
void LevelMakerEnvelope::hpp() {
	float mult = frame::keys::keys[GLFW_KEY_LEFT_CONTROL] ? 10 : frame::keys::keys[GLFW_KEY_LEFT_SHIFT] ? 5 : 1;
	height += mult;
	resizeworld();
}
void LevelMakerEnvelope::wmm() {
	float mult = frame::keys::keys[GLFW_KEY_LEFT_CONTROL] ? 10 : frame::keys::keys[GLFW_KEY_LEFT_SHIFT] ? 5 : 1;
	width -= mult;
	width = (std::max)(width, 1);
	resizeworld();
}
void LevelMakerEnvelope::hmm() {
	float mult = frame::keys::keys[GLFW_KEY_LEFT_CONTROL] ? 10 : frame::keys::keys[GLFW_KEY_LEFT_SHIFT] ? 5 : 1;
	height -= mult;
	height = (std::max)(height, 1);
	resizeworld();
}

void LevelMakerEnvelope::tick() {
	float mult = pow(10, frame::keys::keys[GLFW_KEY_LEFT_CONTROL] - frame::keys::keys[GLFW_KEY_LEFT_SHIFT]);
	if (frame::mouse::m) {
		xOffVel -= ((frame::mouse::x - frame::mouse::pX) * mult) / 4.f;
		yOffVel -= ((frame::mouse::y - frame::mouse::pY) * mult) / 4.f;
	}
	xOff += xOffVel *= 0.8;
	yOff += yOffVel *= 0.8;
	if (frame::keys::keys[GLFW_KEY_LEFT_ALT])
		scroll += (frame::mouse::y_s_g / 10.0) * mult;
	else tile += (int)frame::mouse::y_s;
	tile = (std::min)((std::max)(tile, 1), 2);

	ptX = tileX;
	ptY = tileY;
	tileX = floor((-frame::width / 2 + frame::mouse::x + xOff) / scroll + world.width / 2.f);
	tileY = floor((-frame::height/ 2 + frame::mouse::y + yOff) / scroll + world.height / 2.f);

	menu.tick();
	
	//layer.setPosition(glm::vec3(170, frame::height - 25, 0));
	//addBG.setPosition(glm::vec3(60, frame::height - 25, 0));

	//addBG.tick();
	//layer.tick();
	
	switch (menu.currentTab) {
	case 1:
		menu[1]->at("tile")->setUINT(Quark::TEXTURE, tile == 1 ? frame1 : frame2);
	case 0:
		if ((frame::mouse::left || frame::mouse::right) && tileX >= 0 && tileX < width && tileY >= 0 && tileY < height) {
			s(tileX, tileY, frame::mouse::left * tile, &world);
			if (frame::keys::keys[GLFW_KEY_LEFT_SHIFT] && frame::keys::keys[GLFW_KEY_LEFT_CONTROL])
				for (int i = -100; i < 100; i++)
					for (int j = -100; j < 100; j++)
						s(tileX + i, tileY + j, frame::mouse::left * tile, &world);
			if (frame::keys::keys[GLFW_KEY_LEFT_CONTROL])
				for (int i = -10; i < 10; i++)
					for (int j = -10; j < 10; j++)
						s(tileX + i, tileY + j, frame::mouse::left * tile, &world);
			if (frame::keys::keys[GLFW_KEY_LEFT_SHIFT])
				for (int i = -1; i < 1; i++)
					for (int j = -1; j < 1; j++)
						s(tileX + i, tileY + j, frame::mouse::left * tile, &world);
			world.updateTexture();
		}
		break;
	case 2:
		if (selectedEnt != -1) {
			ent_del.tick();
			if (ent_del.on) {
				entities().erase(entities().begin() + selectedEnt);
				selectedEnt = -1;
				return;
			}
			std::string s = entities.get<CompoundTag>(selectedEnt)->S("type").getValue();
			for (auto i = ent_B[s].begin(); i != ent_B[s].end(); i++) {
				i->second.tick();
				entities.get<CompoundTag>(selectedEnt)->b(i->first)(i->second.on);
			}
			for (auto i = ent_S[s].begin(); i != ent_S[s].end(); i++) {
				i->second.tick(); 
				if (entities.get<CompoundTag>(selectedEnt)->get(i->first)->id() == Tag::FLOAT)
					entities.get<CompoundTag>(selectedEnt)->f(i->first)(i->second.value);
				else if (entities.get<CompoundTag>(selectedEnt)->get(i->first)->id() == Tag::INT)
					entities.get<CompoundTag>(selectedEnt)->i(i->first)(i->second.value);
				else if (entities.get<CompoundTag>(selectedEnt)->get(i->first)->id() == Tag::DOUBLE)
					entities.get<CompoundTag>(selectedEnt)->d(i->first)(i->second.value);
				else if (entities.get<CompoundTag>(selectedEnt)->get(i->first)->id() == Tag::LONG)
					entities.get<CompoundTag>(selectedEnt)->l(i->first)(i->second.value);
				else if (entities.get<CompoundTag>(selectedEnt)->get(i->first)->id() == Tag::SHORT)
					entities.get<CompoundTag>(selectedEnt)->s(i->first)(i->second.value);
			}

			for (auto i = ent_E[s].begin(); i != ent_E[s].end(); i++) {
				i->second.tick();
				entities.get<CompoundTag>(selectedEnt)->S(i->first)(std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().to_bytes(i->second.value));
			}

			int w = 1, h = 1;
			int x = entities.get<CompoundTag>(selectedEnt)->i("x")();
			int y = entities.get<CompoundTag>(selectedEnt)->i("y")();

			switch (hash(entities.get<CompoundTag>(selectedEnt)->S("type")())) {
			case "rock"_:
				w = h = 3;
				break;
			case "button"_:
				w = entities.get<CompoundTag>(selectedEnt)->i("size")();
			case "door"_:
				h = entities.get<CompoundTag>(selectedEnt)->i("size")();
			}
			if (frame::mouse::l && entMove.hovered) {
				x += tileX - ptX;
				y += tileY - ptY;
				entities.get<CompoundTag>(selectedEnt)->i("x")(x);
				entities.get<CompoundTag>(selectedEnt)->i("y")(y);
				entMove = MouseDetector(glm::vec3(x, y, 0), glm::vec3(x + w, y + h, 0));
			}

			if (!frame::mouse::l)
				entMove = MouseDetector(glm::vec3(x, y, 0), glm::vec3(x + w, y + h, 0));
			else
				entMove.tick(tileX, tileY);
			if (frame::mouse::l && frame::selected_component == nullptr && !entMove.hovered)
				selectedEnt = -1;
		}
		if (selectedEnt == -1) {
			// Check 'add entity' buttons
			for (auto i = menu[2]->getObjects().begin(); i != menu[2]->getObjects().end(); i++) {
				MouseDetector b = MouseDetector(i->second->position - i->second->size + glm::vec3(0, 740, 0), i->second->position + i->second->size + glm::vec3(0, 740, 0));
				b.tick();
				if (b.hovered && frame::mouse::l) {
					selectedEnt = entities().size();
					entities.addValue(new CompoundTag(""));
					(*entities.get<CompoundTag>(selectedEnt)) << new IntTag("x", tileX);
					(*entities.get<CompoundTag>(selectedEnt)) << new IntTag("y", tileY);
					(*entities.get<CompoundTag>(selectedEnt)) << new StringTag("type", i->first);
					std::vector<Tag*> s = ent_T[i->first]();
					std::erase_if(s, [](Tag* t) {return std::string(t->getName()).find("~~min") != std::string::npos || std::string(t->getName()).find("~~max") != std::string::npos || std::string(t->getName()).find("~~def") != std::string::npos; });
					for (auto n = s.begin(); n != s.end(); n++)
						switch ((*n)->id()) {
						case Tag::BYTE:
							(*entities.get<CompoundTag>(selectedEnt)) << (dynamic_cast<ByteTag*>(*n)->clone());
							break;
						case Tag::INT:
							(*entities.get<CompoundTag>(selectedEnt)) << (dynamic_cast<IntTag*>(*n)->clone());
							break;
						case Tag::FLOAT:
							(*entities.get<CompoundTag>(selectedEnt)) << (dynamic_cast<FloatTag*>(*n)->clone());
							break;
						case Tag::DOUBLE:
							(*entities.get<CompoundTag>(selectedEnt)) << (dynamic_cast<DoubleTag*>(*n)->clone());
							break;
						case Tag::LONG:
							(*entities.get<CompoundTag>(selectedEnt)) << (dynamic_cast<LongTag*>(*n)->clone());
							break;
						case Tag::SHORT:
							(*entities.get<CompoundTag>(selectedEnt)) << (dynamic_cast<ShortTag*>(*n)->clone());
							break;
						case Tag::STRING:
							(*entities.get<CompoundTag>(selectedEnt)) << (dynamic_cast<StringTag*>(*n)->clone());
							break;
						}
				}
			}
			// Check all unselected entities and see if any of them should be selected
			for (int i = 0; i < entities.getValue().size(); i++) {
				int w = 1, h = 1;
				int x = entities.get<CompoundTag>(i)->i("x")();
				int y = entities.get<CompoundTag>(i)->i("y")();

				switch (hash(entities.get<CompoundTag>(i)->S("type")())) {
				case "rock"_:
					w = h = 3;
					break;
				case "button"_:
					w = entities.get<CompoundTag>(i)->i("size")();
				}
				MouseDetector m = MouseDetector(glm::vec3(x, y, 0), glm::vec3(x + w, y + h, 0));
				m.tick(tileX, tileY);
				if (m.hovered && frame::mouse::l) {
					selectedEnt = i;
					entMove = MouseDetector(glm::vec3(x, y, 0), glm::vec3(x + w, y + h, 0));
					entMove.draggable = true;
					entMove.constrain(glm::vec3(0), glm::vec3(world.width, world.height, 0));
					for (auto i = (*entities.get<CompoundTag>(selectedEnt))().begin(); i != (*entities.get<CompoundTag>(selectedEnt))().end(); i++) {
						double sliderVal = 0;
						std::string name = std::string((*i)->getName());
						if(name!="x" && name != "y" && name != "type")
							switch ((*i)->id()) {
							case Tag::BYTE:
								// Get the button for entity of [type] and then set whether it's on or not to the current Tag's value
								ent_B[entities.get<CompoundTag>(selectedEnt)->S("type")()][(*i)->getName()].setOn(dynamic_cast<ByteTag*>(*i)->getValue());
								std::cout << "Setting button's on value to " << dynamic_cast<ByteTag*>(*i)->getValue() << std::endl;
								break;
							case Tag::STRING:
								// Get the button for entity of [type] and then set the text to the current Tag's value
								ent_E[entities.get<CompoundTag>(selectedEnt)->S("type")()][(*i)->getName()].value = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes((dynamic_cast<StringTag*>(*i)->getValue()));
								ent_E[entities.get<CompoundTag>(selectedEnt)->S("type")()][(*i)->getName()].cursor = dynamic_cast<StringTag*>(*i)->getValue().size();
								ent_E[entities.get<CompoundTag>(selectedEnt)->S("type")()][(*i)->getName()].selectStart = dynamic_cast<StringTag*>(*i)->getValue().size();
								ent_E[entities.get<CompoundTag>(selectedEnt)->S("type")()][(*i)->getName()].freezeTextures();
								std::cout << "Setting textbox's value to " << dynamic_cast<StringTag*>(*i)->getValue() << " : " << std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().to_bytes(ent_E[entities.get<CompoundTag>(selectedEnt)->S("type")()][(*i)->getName()].value) << std::endl;
								break;

							// The following cases sets a slider's value based on which tag type it is
							case Tag::INT:
								sliderVal = dynamic_cast<IntTag*>(*i)->getValue();
								ent_S[entities.get<CompoundTag>(selectedEnt)->S("type")()][(*i)->getName()].value = sliderVal;
								std::cout << "Setting slider's value to " << sliderVal << " : " << ent_S[entities.get<CompoundTag>(selectedEnt)->S("type")()][(*i)->getName()].value << std::endl;
								break;
							case Tag::DOUBLE:
								sliderVal = dynamic_cast<DoubleTag*>(*i)->getValue();
								ent_S[entities.get<CompoundTag>(selectedEnt)->S("type")()][(*i)->getName()].value = sliderVal;
								break;
							case Tag::FLOAT:
								sliderVal = dynamic_cast<FloatTag*>(*i)->getValue();
								ent_S[entities.get<CompoundTag>(selectedEnt)->S("type")()][(*i)->getName()].value = sliderVal;
								break;
							case Tag::LONG:
								sliderVal = dynamic_cast<LongTag*>(*i)->getValue();
								ent_S[entities.get<CompoundTag>(selectedEnt)->S("type")()][(*i)->getName()].value = sliderVal;
								break;
							case Tag::SHORT:
								sliderVal = dynamic_cast<ShortTag*>(*i)->getValue();
								ent_S[entities.get<CompoundTag>(selectedEnt)->S("type")()][(*i)->getName()].value = sliderVal;
								break;
							default:
								std::cout << "Unknown tag of type " << (*i)->id() << ", named " << (*i)->getName() << std::endl;
							}
					}
				}
			}
		}
		if (frame::selected_component != nullptr)
			return;

		if (frame::keys::typed && frame::keys::keys[GLFW_KEY_LEFT_CONTROL] && frame::keys::keys[GLFW_KEY_1])
			entities.addValue(add("rock"));
		else if (frame::keys::typed && frame::keys::keys[GLFW_KEY_LEFT_CONTROL] && frame::keys::keys[GLFW_KEY_2])
			entities.addValue(add("coin"));
		else if (frame::keys::typed && frame::keys::keys[GLFW_KEY_LEFT_CONTROL] && frame::keys::keys[GLFW_KEY_3]) {
			using namespace frame::keys;
			CompoundTag* a = add("button");
			(*a) << new ByteTag("toggle", '\0');

			(*a) << new IntTag("size", 1 + keys[GLFW_KEY_F1] + keys[GLFW_KEY_F2] * 2 + keys[GLFW_KEY_F3] * 4);

			(*a) << new StringTag("event", keys[GLFW_KEY_E] ? "exit" : "?");

			entities.addValue(a);
		}
		else if (frame::keys::typed && frame::keys::keys[GLFW_KEY_LEFT_CONTROL] && frame::keys::keys[GLFW_KEY_4]) {
			using namespace frame::keys;
			CompoundTag* a = add("button");
			(*a) << new ByteTag("toggle", '\0');

			(*a) << new IntTag("size", 1 + keys[GLFW_KEY_F1] + keys[GLFW_KEY_F2] * 2 + keys[GLFW_KEY_F3] * 4);

			(*a) << new StringTag("event", keys[GLFW_KEY_E] ? "exit" : "?");

			entities.addValue(a);
		}
		break;
	}
		

	//LVLMKR_layer = layer.value;
	if (frame::selected_component != nullptr)
		return;

	if (frame::keys::typed && frame::keys::keys[GLFW_KEY_LEFT])
		wmm();
	if (frame::keys::typed && frame::keys::keys[GLFW_KEY_RIGHT])
		wpp();
	if (frame::keys::typed && frame::keys::keys[GLFW_KEY_UP])
		hmm();
	if (frame::keys::typed && frame::keys::keys[GLFW_KEY_DOWN])
		hpp();

	if (frame::keys::typed && frame::keys::keys[GLFW_KEY_V])
		spawn = glm::ivec2(tileX, tileY);
	
	if (frame::keys::typed && frame::keys::keys[GLFW_KEY_S] && frame::keys::keys[GLFW_KEY_LEFT_CONTROL])
		saveImage();
	if (frame::keys::typed && frame::keys::keys[GLFW_KEY_O] && frame::keys::keys[GLFW_KEY_LEFT_CONTROL])
		openWorld();
	if (frame::keys::typed && frame::keys::keys[GLFW_KEY_P] && frame::keys::keys[GLFW_KEY_LEFT_CONTROL])
		saveWorld();
}

void LevelMakerEnvelope::render() {

}

void LevelMakerEnvelope::render_gui() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	passes::globalTransform = processor::form(glm::vec3(frame::width/2.f-xOff, frame::height / 2.f -yOff, 0), glm::vec3(scroll));
	passes::transform = glm::scale(processor::rect(-world.width/2.f, -world.height/2.f, world.width, world.height), glm::vec3(1, -1, 1));
	modes::alpha_test = false;
	modes::gui = true;
	neutron::draw(0, glm::vec4(1));

	for (std::map<float, std::string>::iterator i = bgs.begin(); i != bgs.end(); i++) {
		passes::globalTransform = processor::form(glm::vec3(frame::width/2.f-xOff*(i->first+1), frame::height/ 2.f -yOff * (i->first + 1), 0), glm::vec3(scroll));
		neutron::draw(processor::getTextureFrom(i->second.c_str()));
	}

	passes::globalTransform = processor::form(glm::vec3(frame::width/2.f-xOff, frame::height/ 2.f -yOff, 0), glm::vec3(scroll));

	for (int i = 0; i < world.width; i++) {
		passes::transform = processor::rect(i - world.width / 2.f, -world.height / 2.f, 0.03, world.height);
		neutron::draw(0, glm::vec4(0.5));
	}

	for (int i = 0; i < world.height; i++) {
		passes::transform = processor::rect(-world.width / 2.f, i - world.height / 2.f, world.width, 0.03);
		neutron::draw(0, glm::vec4(0.5));
	}

	passes::transform = glm::scale(processor::rect(-world.width / 2.f, -world.height / 2.f, world.width, world.height), glm::vec3(1, -1, 1));
	neutron::draw(world.texture);

	passes::transform = processor::rect(tileX - world.width / 2.f, tileY - world.height / 2.f, 0.1, 1);
	neutron::draw(0, glm::vec4(0, 0, 1, 1));
	passes::transform = processor::rect(tileX - world.width / 2.f, tileY - world.height / 2.f, 1, 0.1);
	neutron::draw(0, glm::vec4(0, 0, 1, 1));
	passes::transform = processor::rect(tileX - world.width / 2.f + 0.9, tileY - world.height / 2.f, 0.1, 1);
	neutron::draw(0, glm::vec4(0, 0, 1, 1));
	passes::transform = processor::rect(tileX - world.width / 2.f, tileY - world.height / 2.f + 0.9, 1, 0.1);
	neutron::draw(0, glm::vec4(0, 0, 1, 1));

	if (menu.currentTab < 2) {
		passes::transform = processor::rect(tileX - world.width / 2.f, tileY - world.height / 2.f, 1, 1);
		passes::mult_color = glm::vec4(1, 1, 1, 0.5);
		neutron::draw(tile == 1 ? frame1 : frame2);
		passes::mult_color = glm::vec4(1);
	}

	for (int i = 0; i<entities().size(); i++) {
		CompoundTag c = *entities.get<CompoundTag>(i);
		int x = c.i("x")();
		int y = c.i("y")();
		switch (hash(c.S("type")())) {
		case "rock"_:
			passes::transform = processor::rect(x-world.width/2.f, y - world.height / 2.f, 3, 3);
			neutron::draw(processor::getTextureFrom("res/textures/rock.png", GL_NEAREST));
			break;
		case "coin"_:
			passes::transform = processor::rect(x - world.width / 2.f, y - world.height / 2.f, 1, 1);
			neutron::draw(coinTexture);
			break;
		case "button"_:
			if (c.i("size")() == 1) {
				passes::transform = processor::rect(x - world.width / 2.f, y - world.height / 2.f + 0.25, 1, 0.5);
				neutron::draw(button[3]);
				passes::transform = processor::rect(x - world.width / 2.f, y - world.height / 2.f + 0.5, 1, 0.5);
				neutron::draw(button[7]);
			}
			else {
				for (int i = 0; i < c.i("size")(); i++) {
					if (i == 0) {
						passes::transform = processor::rect(x - world.width / 2.f + i, y - world.height / 2.f + 0.25, 1, 0.5);
						neutron::draw(button[0]);
						passes::transform = processor::rect(x - world.width / 2.f + i, y - world.height / 2.f + 0.5, 1, 0.5);
						neutron::draw(button[4]);
					}
					else if(i== c.i("size")()-1) {
						passes::transform = processor::rect(x - world.width / 2.f + i, y - world.height / 2.f + 0.25, 1, 0.5);
						neutron::draw(button[2]);
						passes::transform = processor::rect(x - world.width / 2.f + i, y - world.height / 2.f + 0.5, 1, 0.5);
						neutron::draw(button[6]);
					}
					else {
						passes::transform = processor::rect(x - world.width / 2.f + i, y - world.height / 2.f + 0.25, 1, 0.5);
						neutron::draw(button[1]);
						passes::transform = processor::rect(x - world.width / 2.f + i, y - world.height / 2.f + 0.5, 1, 0.5);
						neutron::draw(button[5]);
					}
				}
			}
			break;
		case "door"_:
			if (c.b("horizontal")()) {
				passes::transform = processor::rect(x - world.width / 2.f, y - world.height / 2.f, c.i("size")(), 1);
				passes::uvTransform = processor::rect(-1, -1, 2 * c.i("size")(), 2, glm::vec3(0, 0, 3.14159265 / 2.0));
				neutron::draw(door1);
				passes::uvTransform = processor::rect(-1, -1, 2, 2, glm::vec3(0, 0, 3.14159265 / 2.0));
				passes::transform = processor::rect(x - world.width / 2.f, y - world.height / 2.f, 1, 1);
				neutron::draw(door0);
				passes::uvTransform = processor::rect(-1, -1, 2, 2);
			}
			else {
				passes::transform = processor::rect(x - world.width / 2.f, y - world.height / 2.f, 1, c.i("size")());
				passes::uvTransform = processor::rect(-1, -1, 2, 2 * c.i("size")());
				neutron::draw(door1);
				passes::uvTransform = processor::rect(-1, -1, 2, 2);
				passes::transform = processor::rect(x - world.width / 2.f, y - world.height / 2.f, 1, 1);
				neutron::draw(door0);
			}
			break;
		}
	}

	passes::transform = processor::form(glm::vec3(spawn.x + 0.5 - world.width / 2.f, spawn.y + 0.5 - world.height/ 2.f, 0), glm::vec3(0.5, 0.5, 0));
	neutron::draw(playerTexture);
	neutron::draw(playerHead);

	passes::globalTransform = glm::mat4(1);
	//addBG.render();
	//layer.render();
	menu.render();

	if (selectedEnt != -1) {
		std::string type = entities.get<CompoundTag>(selectedEnt)->S("type")();
		ent_del.render();
		for (auto b = ent_B[type].begin(); b != ent_B[type].end(); b++)
			b->second.render();
		for (auto b = ent_S[type].begin(); b != ent_S[type].end(); b++)
			b->second.render();
		for (auto b = ent_E[type].begin(); b != ent_E[type].end(); b++)
			b->second.render();
	}
}