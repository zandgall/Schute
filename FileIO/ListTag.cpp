#include "ListTag.h"
#include "FileChooser.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;
ListTag::ListTag() {

}
ListTag::ListTag(std::string name, int type) {
	this->name = name;
	this->type = type;
}

ListTag::~ListTag()
{
}

void ListTag::load(unsigned char* bytes, int index) {
	int namelength = (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
	stringstream ss;
	for (int i = 0; i < namelength; i++) {
		ss << static_cast<char> ((int)bytes[i + index + 3]);
	}
	name = (ss.str());

	index += 3 + namelength;

	type = (int)bytes[index];

	int size = *(int*)(new unsigned char[4]{ bytes[index + 4], bytes[index + 3], bytes[index + 2], bytes[index+1] });
	index += 5;
	// Load each tag in the list
	tags = std::vector<Tag*>();
	while (tags.size() != size) {
		Tag *t;
		switch (type) {
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
		bytes[index - 3] = 0x00;
		bytes[index - 2] = 0x00;
		bytes[index - 1] = 0x00;
		t->load(bytes, index - 3);
		index = t->next(bytes, index - 3);
		tags.push_back(t);
	}
	endIndex = index;
}

std::string ListTag::getCompilation() {
	stringstream ss;
	ss << "ListTag(" << name << "): " << tags.size() << " entries\n{\n";
	for (Tag* i : tags) {
		ss << '\t';
		switch (i->id()) {
		case BYTE:
			ss << "ByteTag(None): " << dynamic_cast<ByteTag*>(i)->getValue();
			break;
		case SHORT:
			ss << "ShortTag(None): " << dynamic_cast<ShortTag*>(i)->getValue();
			break;
		case INT:
			ss << "IntTag(None): " << dynamic_cast<IntTag*>(i)->getValue();
			break;
		case LONG:
			ss << "LongTag(None): " << dynamic_cast<LongTag*>(i)->getValue();
			break;
		case FLOAT:
			ss << "FloatTag(None): " << dynamic_cast<FloatTag*>(i)->getValue();
			break;
		case DOUBLE:
			ss << "DoubleTag(None): " << dynamic_cast<DoubleTag*>(i)->getValue();
			break;
		case BYTE_ARRAY:
			ss << "ByteArrayTag(None): [" << dynamic_cast<ByteArrayTag*>(i)->getValue().size() << " bytes]";
			break;
		case STRING:
			ss << "StringTag(None): " << dynamic_cast<StringTag*>(i)->getValue();
			break;
		case LIST: {
			string listV = dynamic_cast<ListTag*>(i)->getCompilation();
			std::stringstream s(listV);
			std::string token;
			std::string t = "";
			while (getline(s, token, '\n')) {
				ss << t << token << "\n";
				t = "\t";
			}
			break;
		}
		case COMPOUND: {
			string listV = dynamic_cast<CompoundTag*>(i)->getCompilation(false);
			std::stringstream s(listV);
			std::string token;
			std::string t = "";
			while (getline(s, token, '\n')) {
				ss << t << token << "\n";
				t = "\t";
			}
			break;
		}
		case INT_ARRAY:
			ss << "IntArrayTag(None): [" << dynamic_cast<ByteArrayTag*>(i)->getValue().size() << " ints]";
			break;
		case LONG_ARRAY:
			ss << "LongArrayTag(None): [" << dynamic_cast<ByteArrayTag*>(i)->getValue().size() << " longs]";
			break;
		case FLOAT_ARRAY:
			ss << "FloatArrayTag(None): [" << dynamic_cast<ByteArrayTag*>(i)->getValue().size() << " floats]";
			break;
		case DOUBLE_ARRAY:
			ss << "DoubleArrayTag(None): [" << dynamic_cast<ByteArrayTag*>(i)->getValue().size() << " doubles]";
			break;
		}
		ss << "\n";
	}
	ss << "}";
	return ss.str();
}
int ListTag::next(unsigned char* bytes, int index) {
	return endIndex;
}

vector<unsigned char> ListTag::getBytesValue() {
	std::vector<unsigned char> out = std::vector<unsigned char>();
	for (Tag* tag : tags) {
		tag->setName("");
		std::vector<unsigned char> tag_val = tag->outBytes();
		out.insert(out.end(), tag_val.begin() + 3, tag_val.end());
	}
	return out;
}