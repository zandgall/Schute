#include "ByteTag.h"
#include "FileChooser.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;
ByteTag::ByteTag()
{
}


ByteTag::~ByteTag()
{
}

void ByteTag::load(unsigned char* bytes, int index) {
	int namelength = (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
	stringstream ss;
	for (int i = 0; i < namelength; i++) {
		ss << bytes[i + index + 3];
	}
	name = (ss.str());

	index += 3 + namelength;
	data = bytes[index];
}

int ByteTag::next(unsigned char* bytes, int index) {
	return index + 4 + (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
}

vector<unsigned char> ByteTag::getBytesValue() {
	return vector<unsigned char>{data};
}