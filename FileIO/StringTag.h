#pragma once
#include "Tag.h"
class StringTag : virtual public Tag {
public:
	StringTag();
	StringTag(std::string name, std::string data) {
		this->name = name;
		this->data = data;
	};
	~StringTag();
	void load(unsigned char* bytes, int index);
	int next(unsigned char* bytes, int index);
	std::string& getValue() {
		return data;
	}
	std::string& operator()() {
		return data;
	}
	void operator()(std::string b) {
		data = b;
	}
	std::vector<unsigned char> getBytesValue();
	std::vector<unsigned char> outBytes() {
		std::vector<unsigned char> out = std::vector<unsigned char>();
		out.push_back(0x08);
		int nS = name.size();
		out.push_back((nS & 0x00'00'ff'00) >> 8);
		out.push_back(nS & 0x00'00'00'ff);
		for (int i = 0; i < nS; i++)
			out.push_back((char)name.at(i));
		out.push_back((data.size() & 0x00'00'ff'00) >> 8);
		out.push_back(data.size() & 0x00'00'00'ff);
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
		return 8;
	}
	Tag* clone() {
		return new StringTag(name, data);
	}
private:
	std::string name;
	std::string data;
};

