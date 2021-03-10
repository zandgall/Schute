#include "DoubleArrayTag.h"
#include "FileChooser.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

DoubleArrayTag::DoubleArrayTag()
{
}


DoubleArrayTag::~DoubleArrayTag()
{
}

void DoubleArrayTag::load(unsigned char* bytes, int index) {
	int namelength = (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2];
	stringstream ss;
	for (int i = 0; i < namelength; i++)
		ss << bytes[i + index + 3];
	name = (ss.str());

	index += 3 + namelength;

	int size = *(int*)(new unsigned char[4]{ bytes[index + 3], bytes[index + 2], bytes[index + 1], bytes[index] });
	data = vector<double>();
	for (int i = 0; i < size; i++)
		data.push_back(*(double*)(new unsigned char[8]{ bytes[index + i*8 + 11], bytes[index + i*8 + 10], bytes[index + i*8 + 9], bytes[index + i*8 + 8], bytes[index + i * 8 + 7], bytes[index + i * 8 + 6], bytes[index + i * 8 + 5], bytes[index + i * 8 + 4] }));
}

int DoubleArrayTag::next(unsigned char* bytes, int index) {
	return index + 7 + (int)(bytes[index + 1] * 16 * 16) + (int)bytes[index + 2] + data.size()*8;
}

vector<unsigned char> DoubleArrayTag::getBytesValue() {
	vector<unsigned char> out = vector<unsigned char>();
	union u_double {
		double f;
		char data[sizeof(double)];
	};
	for (int i = 0; i < data.size(); i++) {
		u_double f;
		f.f = data[i];

		out.push_back(f.data[7]);
		out.push_back(f.data[6]);
		out.push_back(f.data[5]);
		out.push_back(f.data[4]);
		out.push_back(f.data[3]);
		out.push_back(f.data[2]);
		out.push_back(f.data[1]);
		out.push_back(f.data[0]);
	}
	return out;
}