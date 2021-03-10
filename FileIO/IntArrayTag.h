#pragma once
#include "Tag.h"
class IntArrayTag : virtual public Tag {
public:
	IntArrayTag();
	IntArrayTag(std::string name, std::vector<int> data) {
		this->name = name;
		this->data = data;
	};
	IntArrayTag(std::string name, int data[]) {
		this->name = name;
		this->data = std::vector<int>();
		for (int i = 0, size = sizeof(data) / sizeof(int); i < size; i++)
			this->data.push_back(data[i]);
	};
	~IntArrayTag();
	void load(unsigned char* bytes, int index);
	int next(unsigned char* bytes, int index);
	std::vector<int>& getValue() {
		return data;
	}
	std::vector<int>& operator()() {
		return data;
	}
	void operator()(std::vector<int> b) {
		data = b;
	}
	int& operator[](size_t i) {
		return data[i];
	}
	std::vector<unsigned char> getBytesValue();
	std::vector<unsigned char> outBytes() {
		std::vector<unsigned char> out = std::vector<unsigned char>();
		out.push_back(0x0B);
		int nS = name.size();
		out.push_back((nS & 0x00'00'ff'00) >> 16);
		out.push_back(nS & 0x00'00'00'ff);
		for (int i = 0; i < nS; i++)
			out.push_back((char)name.at(i));
		out.push_back((data.size() & 0xff'00'00'00) >> 24);
		out.push_back((data.size() & 0x00'ff'00'00) >> 16);
		out.push_back((data.size() & 0x00'00'ff'00) >> 8);
		out.push_back(data.size() & 0x00'00'00'ff);
		std::vector<unsigned char> val = getBytesValue();
		out.insert(out.end(), val.begin(), val.end());
		return out;
	};
	const char* getName() {
		return name.c_str();
	}
	void setName(std::string name) {
		this->name = name;
	}
	unsigned int id() {
		return 11;
	}
	Tag* clone() {
		return new IntArrayTag(name, data);
	}
private:
	std::string name;
	std::vector<int> data;
};

