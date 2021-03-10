#include "Tag.h"
#pragma once
class LongTag : virtual public Tag {
public:
	LongTag();
	LongTag(std::string name, long long data) {
		this->name = name;
		this->data = data;
	};
	~LongTag();
	void load(unsigned char* bytes, int index);
	int next(unsigned char* bytes, int index);
	long long& getValue() {
		return data;
	}
	long long& operator()() {
		return data;
	}
	void operator()(long long b) {
		data = b;
	}
	std::vector<unsigned char> getBytesValue();
	std::vector<unsigned char> outBytes() {
		std::vector<unsigned char> out = std::vector<unsigned char>();
		out.push_back(0x04);
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
		return 4;
	}
	Tag* clone() {
		return new LongTag(name, data);
	}
private:
	std::string name;
	long long data;
};

