#include "ByteArrayTag.h"
#include <string>
#include <iostream>
#include <sstream>
using namespace std;


ByteArrayTag::ByteArrayTag()
{
}


ByteArrayTag::~ByteArrayTag()
{
}

void ByteArrayTag::load(unsigned char* bytes, int index) {
	cout << "Loading unsigned char array";
	int namelength = (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
	cout << ": " << namelength;
	stringstream ss;
	for (int i = 0; i < namelength; i++) {
		ss << static_cast<char> ((int)bytes[i + index + 3]);
		cout << static_cast<char> ((int)bytes[i + index + 3]);
	}
	name = (ss.str());

	cout << ": " << name;
	index += 3 + namelength;

	int size = (int)bytes[index]*16*16*16*16*16*16 + (int)bytes[index+1]*16*16*16*16 + (int)bytes[index+2]*16*16+ (int)bytes[index+3];
	cout << " (" << size << "b) " << (int) bytes[index] << " : " << (int) bytes[index+1] << " : " << (int) bytes[index+2] << " : " << (int) bytes[index+3] << endl;
	data = vector<unsigned char>();
	for (int i = 0; i < size; i++) {
		data.push_back(bytes[index + i + 4]);
	}
}

int ByteArrayTag::next(unsigned char* bytes, int index) {
	return index + 7 + (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2] + data.size();
}

vector<unsigned char> ByteArrayTag::getBytesValue() {
	return data;
}