#pragma once
#include "Tag.h"
class IntTag : virtual public Tag
{
public:
	IntTag();
	IntTag(std::string name, int data) {
		this->name = name;
		this->data = data;
	}
	~IntTag();
	void load(unsigned char* bytes, int index);
	int next(unsigned char* bytes, int index);
	int& getValue() {
		return data;
	}
	int& operator()() {
		return data;
	}
	void operator()(int b) {
		data = b;
	}
	std::vector<unsigned char> getBytesValue();
	std::vector<unsigned char> outBytes() {
		std::vector<unsigned char> out = std::vector<unsigned char>();
		out.push_back(0x03);
		int nS = name.size();
		out.push_back((nS & 0x00'00'ff'00) >> 8);
		out.push_back(nS & 0x00'00'00'ff);
		for (int i = 0; i < nS; i++)
			out.push_back((char)name.at(i));
		for (int i = 0; i < getBytesValue().size(); i++)
			out.push_back(getBytesValue().at(i));
		return out;
	};
	const char* getName() {
		return name.c_str();
	}
	void setName(std::string name) {
		this->name = name;
	}
	unsigned int id() {
		return 3;
	}
	Tag* clone() {
		return new IntTag(name, data);
	}
private:
	std::string name;
	int data;
};

