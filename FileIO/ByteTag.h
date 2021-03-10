#include "Tag.h"
#pragma once
class ByteTag : virtual public Tag {
public:
	ByteTag();
	ByteTag(std::string name, unsigned char data) {
		this->name = name;
		this->data = data;
	}
	ByteTag(std::string name, char data) {
		this->name = name;
		this->data = data;
	}
	~ByteTag();
	void load(unsigned char* bytes, int index);
	int next(unsigned char* bytes, int index);
	unsigned char& getValue() {
		return data;
	}
	unsigned char& operator()() {
		return data;
	}
	void operator()(unsigned char b) {
		data = b;
	}
	std::vector<unsigned char> getBytesValue();
	std::vector<unsigned char> outBytes() {
		std::vector<unsigned char> out = std::vector<unsigned char>();
		out.push_back(0x01);
		int nS = name.size();
		out.push_back((nS & 0x00'00'ff'00) >> 16);
		out.push_back(nS & 0x00'00'00'ff);
		for (int i = 0; i < nS; i++)
			out.push_back((char)name.at(i));
		out.push_back(data);
		return out;
	};
	const char* getName() {
		return name.c_str();
	}
	void setName(std::string name) {
		this->name = name;
	}
	unsigned int id() {
		return 1;
	}
	Tag* clone() {
		return new ByteTag(name, data);
	}
private:
	std::string name;
	unsigned char data;
};

