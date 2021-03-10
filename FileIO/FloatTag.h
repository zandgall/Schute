#pragma once
#include "Tag.h"
class FloatTag : virtual public Tag {
public:
	FloatTag();
	FloatTag(std::string name, float data) {
		this->name = name;
		this->data = data;
	};
	~FloatTag();
	void load(unsigned char* bytes, int index);
	int next(unsigned char* bytes, int index);
	float& getValue() {
		return data;
	}
	float& operator()() {
		return data;
	}
	void operator()(float b) {
		data = b;
	}
	std::vector<unsigned char> getBytesValue();
	std::vector<unsigned char> outBytes() {
		std::vector<unsigned char> out = std::vector<unsigned char>();
		out.push_back(0x05);
		int nS = name.size();
		out.push_back((nS & 0x00'00'ff'00) >> 16);
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
		return 5;
	}
	Tag* clone() {
		return new FloatTag(name, data);
	}
private:
	std::string name;
	float data;
};

