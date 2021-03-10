#include "CompoundTag.h"

#include "FileChooser.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <Windows.h>

using namespace std;
CompoundTag::CompoundTag(){
	tags = std::vector<Tag*>(); 
}


CompoundTag::~CompoundTag(){
	tags.clear();
	tags.~vector();
}

void CompoundTag::load(unsigned char* bytes, int index) {
	int namelength = (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
	stringstream ss;
	for (int i = 0; i < namelength; i++) {
		ss << static_cast<char> ((int)bytes[i + index + 3]);
	}
	name = ss.str();
	index += 3 + namelength;

	for (index; true;) {
		int TId = bytes[index];
		Tag *t;
		switch (TId) {
		case END:
			endIndex = index;
			return;
		case BYTE:
			t = new ByteTag();
			break;
		case SHORT:
			t = new ShortTag();
			break;
		case INT:
			t = new IntTag();
			break;
		case LONG:
			t = new LongTag();
			break;
		case FLOAT:
			t = new FloatTag();
			break;
		case DOUBLE:
			t = new DoubleTag();
			break;
		case BYTE_ARRAY:
			t = new ByteArrayTag();
			break;
		case STRING:
			t = new StringTag();
			break;
		case LIST:
			t = new ListTag();
			break;
		case COMPOUND:
			t = new CompoundTag();
			break;
		case INT_ARRAY:
			t = new IntArrayTag();
			break;
		case LONG_ARRAY:
			t = new LongArrayTag();
			break;
		case FLOAT_ARRAY:
			t = new FloatArrayTag();
			break;
		case DOUBLE_ARRAY:
			t = new DoubleArrayTag();
			break;
		default:
			std::cout << "ERROR: NO VALID TAG_ID FOUND! From CompoundTag(" << name << "): " << index << std::endl;
			return;
		}
		t->load(bytes, index);
		index = t->next(bytes, index);
		tags.push_back(t);
	}
	endIndex = index;
}

string CompoundTag::getCompilation(bool print) {
	stringstream ss;
	ss << "CompoundTag(" << name << "): " << tags.size() << " entries\n{\n";
	for (Tag* i : tags) {
		ss << '\t';
		switch (i->id()) {
		case BYTE:
			ss << "ByteTag(" << i->getName() << "): " << dynamic_cast<ByteTag*>(i)->getValue();
			break;
		case SHORT:
			ss << "ShortTag(" << i->getName() << "): " << dynamic_cast<ShortTag*>(i)->getValue();
			break;
		case INT:
			ss << "IntTag(" << i->getName() << "): " << dynamic_cast<IntTag*>(i)->getValue();
			break;
		case LONG:
			ss << "LongTag(" << i->getName() << "): " << dynamic_cast<LongTag*>(i)->getValue();
			break;
		case FLOAT:
			ss << "FloatTag(" << i->getName() << "): " << dynamic_cast<FloatTag*>(i)->getValue();
			break;
		case DOUBLE:
			ss << "DoubleTag(" << i->getName() << "): " << dynamic_cast<DoubleTag*>(i)->getValue();
			break;
		case BYTE_ARRAY:
			ss << "ByteArrayTag(" << i->getName() << "): [" << dynamic_cast<ByteArrayTag*>(i)->getValue().size() << " bytes]";
			break;
		case STRING:
			ss << "StringTag(" << i->getName() << "): " << dynamic_cast<StringTag*>(i)->getValue();
			break;
		case LIST: {
			string listV = dynamic_cast<ListTag*>(i)->getCompilation();
			stringstream s(listV);
			string token;
			string t = "";
			while (getline(s, token, '\n')) {
				ss << t << token << "\n";
				t = "\t";
			}
			break;
		}
		case COMPOUND: {
			string listV = dynamic_cast<CompoundTag*>(i)->getCompilation(false);
			stringstream s(listV);
			string token;
			string t = "";
			while (getline(s, token, '\n')) {
				ss << t << token << "\n";
				t = "\t";
			}
			break;
		}
		case INT_ARRAY:
			ss << "IntArrayTag(" << i->getName() << "): [" << dynamic_cast<IntArrayTag*>(i)->getValue().size() << " ints]";
			break;
		case LONG_ARRAY:
			ss << "LongArrayTag(" << i->getName() << "): [" << dynamic_cast<LongArrayTag*>(i)->getValue().size() << " longs]";
			break;
		case FLOAT_ARRAY:
			ss << "FloatArrayTag(" << i->getName() << "): [" << dynamic_cast<FloatArrayTag*>(i)->getValue().size() << " floats]";
			break;
		case DOUBLE_ARRAY:
			ss << "DoubleArrayTag(" << i->getName() << "): [" << dynamic_cast<DoubleArrayTag*>(i)->getValue().size() << " doubles]";
			break;
		}
		ss << "\n";
	}
	ss << "}";
	if (print)
		cout << ss.str() << endl;
	return ss.str();
}

int CompoundTag::next(unsigned char* bytes, int index) {
	return endIndex+1;
}

const vector<Tag*>& CompoundTag::getValue() {
	return tags;
}

Tag *CompoundTag::get(std::string name) {
	for (int i = 0; i < tags.size(); i++) {
		Tag *tag = tags.at(i);
		if (tag->getName() == name)
			return tag;
	}
	return nullptr;
}

std::vector<unsigned char> CompoundTag::getBytesValue() {
	return vector<unsigned char>{ 00 };
}
vector<unsigned char> CompoundTag::outBytes() {
	vector<unsigned char> out = vector<unsigned char>();

	out.push_back(0x0A);
	out.push_back((unsigned char)(name.size() & 0x00'00'ff'00) >> 8);
	out.push_back((unsigned char)(name.size() & 0x00'00'00'ff));

	for (int i = 0; i < name.size(); i++) {
		out.push_back((int)name.at(i));
	}
	
	for (int i = 0; i < tags.size(); i++) {
		Tag *tag = tags[i]; 
		cout << "tag: " << i << " " << tag->getName() << endl;
		std::vector<unsigned char> bytes = tag->outBytes();
		out.insert(out.end(), bytes.begin(), bytes.end());
	}
	
	out.push_back(0x00);
	
	return out;
}


ByteArrayTag *CompoundTag::getByteArrayTag(std::string name) {
	Tag* t = get(name);
	if(t == nullptr || t->id() != 7) {
		std::cout << "There's no ByteArrayTag of this name! \"" << name << "\"" << std::endl;
		return nullptr;
	}
	return dynamic_cast<ByteArrayTag*>(t);
}
ByteTag *CompoundTag::getByteTag(std::string name) {
	Tag* t = get(name);
	if(t == nullptr || t->id() != 1) {
		std::cout << "There's no ByteTag of this name! \"" << name << "\"" << std::endl;
		return nullptr;
	}
	return dynamic_cast<ByteTag*>(t);
}
DoubleTag *CompoundTag::getDoubleTag(std::string name) {
	Tag* t = get(name);
	if(t == nullptr || t->id() != 6) {
		std::cout << "There's no DoubleTag of this name! \"" << name << "\"" << std::endl;
		return nullptr;
	}
	return dynamic_cast<DoubleTag*>(t);
}
FloatTag *CompoundTag::getFloatTag(std::string name) {
	Tag* t = get(name);
	if(t == nullptr || t->id() != 5) {
		std::cout << "There's no FloatTag of this name! \"" << name << "\"" << std::endl;
		return nullptr;
	}
	return dynamic_cast<FloatTag*>(t);
}
IntArrayTag *CompoundTag::getIntArrayTag(std::string name) {
	Tag* t = get(name);
	if(t == nullptr || t->id() != 11) {
		std::cout << "There's no IntArrayTag of this name! \"" << name << "\"" << std::endl;
		return nullptr;
	}
	return dynamic_cast<IntArrayTag*>(t);
}
IntTag *CompoundTag::getIntTag(std::string name) {
	Tag* t = get(name);
	if(t == nullptr || t->id() != 3) {
		std::cout << "There's no IntTag of this name! \"" << name << "\"" << std::endl;
		return nullptr;
	}
	return dynamic_cast<IntTag*>(t);
}

ListTag *CompoundTag::getListTag(std::string name) {
	Tag* t = get(name);
	if(t == nullptr || t->id() != 9) {
		std::cout << "There's no ListTag of this name! \"" << name << "\"" << std::endl;
		return nullptr;
	}
	return dynamic_cast<ListTag*>(t);
}
LongArrayTag *CompoundTag::getLongArrayTag(std::string name) {
	Tag* t = get(name);
	if(t == nullptr || t->id() != 12) {
		std::cout << "There's no LongArrayTag of this name! \"" << name << "\"" << std::endl;
		return nullptr;
	}
	return dynamic_cast<LongArrayTag*>(t);
}
LongTag *CompoundTag::getLongTag(std::string name) {
	Tag* t = get(name);
	if(t == nullptr || t->id() != 4) {
		std::cout << "There's no LongTag of this name! \"" << name << "\"" << std::endl;
		return nullptr;
	}
	return dynamic_cast<LongTag*>(t);
}
ShortTag *CompoundTag::getShortTag(std::string name) {
	Tag* t = get(name);
	if(t == nullptr || t->id() != 2) {
		std::cout << "There's no ShortTag of this name! \"" << name << "\"" << std::endl;
		return nullptr;
	}
	return dynamic_cast<ShortTag*>(t);
}
StringTag *CompoundTag::getStringTag(std::string name) {
	Tag* t = get(name);
	if(t == nullptr || t->id() != 8) {
		std::cout << "There's no StringTag of this name! \"" << name << "\"" << std::endl;
		return nullptr;
	}
	return dynamic_cast<StringTag*>(t);
}

CompoundTag *CompoundTag::getCompoundTag(std::string name) {
	Tag* t = get(name);
	if(t == nullptr || t->id() != 10) {
		std::cout << "There's no CompoundTag of this name! \"" << name << "\"" << std::endl;
		return nullptr;
	}
	return dynamic_cast<CompoundTag*>(t);
}

FloatArrayTag *CompoundTag::getFloatArrayTag(std::string name) {
	Tag* t = get(name);
	if (t == nullptr || t->id() != FLOAT_ARRAY) {
		std::cout << "There's no FloatArrayTag of this name! \"" << name << "\"" << std::endl;
		return nullptr;
	}
	return dynamic_cast<FloatArrayTag*>(t);
}

DoubleArrayTag *CompoundTag::getDoubleArrayTag(std::string name) {
	Tag* t = get(name);
	if(t == nullptr || t->id() != DOUBLE_ARRAY) {
		std::cout << "There's no DoubleArrayTag of this name! \"" << name << "\"" << std::endl;
		return nullptr;
	}
	return dynamic_cast<DoubleArrayTag*>(t);
}