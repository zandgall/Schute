#include "StringTag.h"
#include "FileChooser.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

StringTag::StringTag()
{
}


StringTag::~StringTag()
{
}

void StringTag::load(unsigned char* bytes, int index) {
	int namelength = (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
	stringstream ss;
	for (int i = 0; i < namelength; i++) {
		ss << static_cast<char> ((int)bytes[i + index + 3]);
	}
	name = (ss.str());

	index += 3 + namelength;
	int size = (int)(bytes[index] * 16 * 16) + (int)bytes[index + 1];
	stringstream ss2;
	for (int i = 0; i < size; i++) {
		ss2 << static_cast<char> ((int)bytes[i + index + 2]);
	}
	data = ss2.str();
}

int StringTag::next(unsigned char* bytes, int index) {
	return index + 5 + (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2] + data.size();
}

vector<unsigned char> StringTag::getBytesValue() {
	vector<unsigned char> out;
	for (int i = 0; i < data.size(); i++)
		out.push_back((char)data.at(i));
	return out;
}