#pragma once
#include "Tag.h"
#include <string>
class ShortTag : virtual public Tag {
public:
	ShortTag();
	ShortTag(std::string name, short data) {
		this->name = name;
		this->data = data;
	};
	~ShortTag();
	void load(unsigned char* bytes, int index);
	int next(unsigned char* bytes, int index);
	short& getValue() {
		return data;
	}
	short& operator()() {
		return data;
	}
	void operator()(short b) {
		data = b;
	}
	std::vector<unsigned char> getBytesValue();
	std::vector<unsigned char> outBytes() {
		std::vector<unsigned char> out = std::vector<unsigned char>();
		out.push_back(0x02);
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
		return 2;
	}
	Tag* clone() {
		return new ShortTag(name, data);
	}
private:
	std::string name;
	short data;
};

