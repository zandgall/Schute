#include "ShortTag.h"
#include "FileChooser.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

ShortTag::ShortTag(){
}


ShortTag::~ShortTag(){
}

void ShortTag::load(unsigned char* bytes, int index) {
	cout << "Loading short";
	int namelength = (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
	cout << ": " << namelength;
	stringstream ss;
	for (int i = 0; i < namelength; i++) {
		ss << static_cast<char> ((int)bytes[i + index + 3]);
		cout << static_cast<char> ((int)bytes[i + index + 3]);
	}
	name = (ss.str());

	cout << ": " << name << " ("  << ss.str() << ")" << endl;
	index += 3 + namelength;
	data = (int)(bytes[index] * 16 * 16) + (int)bytes[index + 1];
}

int ShortTag::next(unsigned char* bytes, int index) {
	return index + 5 + (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
}

vector<unsigned char> ShortTag::getBytesValue() {
	vector<unsigned char> out;
	out.push_back((data & 0xff'00) >> 8);
	out.push_back(data & 0x00'ff);
	return out;
}