#pragma once
#include "Tag.h"
class ListTag : virtual public Tag {
public:
	ListTag();
	ListTag(std::string name, int type);
	~ListTag();
	void load(unsigned char* bytes, int index);
	int next(unsigned char* bytes, int index);
	
	std::vector<Tag*>& getValue() {
		return tags;
	}
	std::vector<Tag*>& operator()() {
		return tags;
	}
	void operator()(std::vector<Tag*> b) {
		tags = b;
	}
	template <typename T = Tag>
	T* get(size_t i) {
		return dynamic_cast<T*>(tags[i]);
	}
	Tag* operator[](size_t i) {
		return tags[i];
	}
	std::vector<unsigned char> getBytesValue();
	std::vector<unsigned char> outBytes() {
		std::vector<unsigned char> out = std::vector<unsigned char>();
		out.push_back(0x09);
		int nS = name.size();
		out.push_back((nS & 0x00'00'ff'00) >> 8);
		out.push_back(nS & 0x00'00'00'ff);
		for (int i = 0; i < nS; i++)
			out.push_back((char)name.at(i));
		out.push_back(type & 0x00'00'00'ff);
		out.push_back((tags.size() & 0xff'00'00'00) >> 24);
		out.push_back((tags.size() & 0x00'ff'00'00) >> 16);
		out.push_back((tags.size() & 0x00'00'ff'00) >> 8);
		out.push_back((tags.size() & 0x00'00'00'ff));
		std::vector<unsigned char> val = getBytesValue();
		out.insert(out.end(), val.begin(), val.end());
		return out;
	};
	std::string getCompilation();
	std::vector<Tag*> *getTags() {
		return &tags;
	}
	void addValue(Tag* s) {
		tags.push_back(s);
	}
	const char* getName() {
		return name.c_str();
	}
	void setName(std::string name) {
		this->name = name;
	}
	unsigned int id() {
		return LIST;
	}
	int type;
	Tag* clone() {
		ListTag* out = new ListTag(name, type);
		for (int i = 0; i < tags.size(); i++)
			out->addValue(tags[i]->clone());
		return out;
	}
private:
	std::string name;
	std::vector<Tag*> tags;
	int endIndex = 0;
};
