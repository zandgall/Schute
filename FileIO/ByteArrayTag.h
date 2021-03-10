#pragma once
#ifndef BYTEARRAYTAG_H
#define BYTEARRAYTAG_H
#include "Tag.h"
#include <vector>
class ByteArrayTag : virtual public Tag {
public:
	ByteArrayTag();
	ByteArrayTag(std::string name, std::vector<unsigned char> data) {
		this->name = name;
		this->data = data;
	};
	ByteArrayTag(std::string name, unsigned char data[]) {
		this->data = std::vector<unsigned char>();
		this->name = name;
		for (int i = 0, size = sizeof(data); i < size; i++)
			this->data.push_back(data[i]);
	};
	ByteArrayTag(std::string name, const char* data, int size) {
		this->data = std::vector<unsigned char>();
		this->name = name;
		for (int i = 0; i < size; i++)
			this->data.push_back(data[i]);
	}
	~ByteArrayTag();
	void load(unsigned char* bytes, int index);
	int next(unsigned char* bytes, int index);
	std::vector<unsigned char>& getValue() {
		return data;
	}
	std::vector<unsigned char>& operator()() {
		return data;
	}
	void operator()(std::vector<unsigned char> b) {
		data = b;
	}
	unsigned char& operator[](size_t i) {
		return data[i];
	}
	std::vector<unsigned char> getBytesValue();
	std::vector<unsigned char> outBytes() {
		std::vector<unsigned char> out = std::vector<unsigned char>();
		out.push_back(0x07);
		int nS = name.size();
		out.push_back((nS & 0x00'00'ff'00)>>16);
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
		return 7;
	}
	Tag* clone() {
		return new ByteArrayTag(name, data);
	}
private:
	std::string name;
	std::vector<unsigned char> data;
};
#endif