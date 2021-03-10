#include "FloatArrayTag.h"
#include "FileChooser.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

FloatArrayTag::FloatArrayTag()
{
}


FloatArrayTag::~FloatArrayTag()
{
}

void FloatArrayTag::load(unsigned char* bytes, int index) {
	int namelength = (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
	stringstream ss;
	for (int i = 0; i < namelength; i++)
		ss << bytes[i + index + 3];
	name = (ss.str());

	index += 3 + namelength;

	int size = *(int*)(new unsigned char[4]{ bytes[index + 3], bytes[index + 2], bytes[index + 1], bytes[index] });
	data = vector<float>();
	for (int i = 0; i < size; i++)
		data.push_back(*(float*)(new unsigned char[4]{ bytes[index + i*4 + 7], bytes[index + i*4 + 6], bytes[index + i*4 + 5], bytes[index + i*4 + 4] }));
}

int FloatArrayTag::next(unsigned char* bytes, int index) {
	return index + 7 + (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2] + data.size()*4;
}

vector<unsigned char> FloatArrayTag::getBytesValue() {
	vector<unsigned char> out = vector<unsigned char>();
	union u_float {
		float f;
		char data[sizeof(float)];
	};
	for (int i = 0; i < data.size(); i++) {
		u_float f;
		f.f = data[i];

		out.push_back(f.data[3]);
		out.push_back(f.data[2]);
		out.push_back(f.data[1]);
		out.push_back(f.data[0]);
	}
	return out;
}