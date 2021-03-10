#pragma once

#ifndef COMPOUNDTAG_H
#define COMPOUNDTAG_H

#include "Tag.h"
#include "ByteArrayTag.h"
#include "ByteTag.h"
#include "DoubleTag.h"
#include "FloatTag.h"
#include "IntArrayTag.h"
#include "IntTag.h"
#include "ListTag.h"
#include "LongArrayTag.h"
#include "LongTag.h"
#include "ShortTag.h"
#include "StringTag.h"
#include "FloatArrayTag.h"
#include "DoubleArrayTag.h"
#include <vector>
#include <string>
class CompoundTag : virtual public Tag {
public:
	std::vector<Tag*> tags;
	CompoundTag();
	~CompoundTag();
	CompoundTag(std::string name) {
		this->name = name;
	}
	void load(unsigned char* bytes, int index);
	int next(unsigned char* bytes, int index);
	const std::vector<Tag*>& getValue();
	std::string getCompilation(bool print = true);
	std::vector<unsigned char> getBytesValue();
	std::vector<unsigned char> outBytes();
	Tag *get(std::string name);
	const char* getName() {
		return name.c_str();
	}
	void setName(std::string name) {
		this->name = name;
	}
	unsigned int id() {
		return 10;
	}

	void operator <<(Tag* tag) {
		tags.push_back(tag);
	}
	
	template <typename T = Tag>
	T* get(std::string name) {
		return dynamic_cast<T*>(tags[name]);
	}
	Tag* operator [](std::string name) {
		return get(name);
	}

	std::vector<Tag*> &operator ()() {
		return tags;
	}

	ByteArrayTag *getByteArrayTag(std::string name);
	ByteArrayTag &B(std::string name) {
		return *getByteArrayTag(name);
	}
	ByteTag *getByteTag(std::string name);
	ByteTag &b(std::string name) {
		return *getByteTag(name);
	}
	DoubleTag *getDoubleTag(std::string name);
	DoubleTag &d(std::string name) {
		return *getDoubleTag(name);
	}
	FloatTag *getFloatTag(std::string name);
	FloatTag &f(std::string name) {
		return *getFloatTag(name);
	}
	IntArrayTag *getIntArrayTag(std::string name);
	IntArrayTag &I(std::string name) {
		return *getIntArrayTag(name);
	}
	IntTag *getIntTag(std::string name);
	IntTag &i(std::string name) {
		return *getIntTag(name);
	}

	ListTag *getListTag(std::string name);
	ListTag &li(std::string name) {
		return *getListTag(name);
	}
	LongArrayTag *getLongArrayTag(std::string name);
	LongArrayTag &L(std::string name) {
		return *getLongArrayTag(name);
	}
	LongTag *getLongTag(std::string name);
	LongTag &l(std::string name) {
		return *getLongTag(name);
	}
	ShortTag *getShortTag(std::string name);
	ShortTag &s(std::string name) {
		return *getShortTag(name);
	}
	StringTag *getStringTag(std::string name);
	StringTag &S(std::string name) {
		return *getStringTag(name);
	}
	CompoundTag *getCompoundTag(std::string name);
	CompoundTag &c(std::string name) {
		return *getCompoundTag(name);
	}
	FloatArrayTag *getFloatArrayTag(std::string name);
	FloatArrayTag &F(std::string name) {
		return *getFloatArrayTag(name);
	}
	DoubleArrayTag *getDoubleArrayTag(std::string name);
	DoubleArrayTag &D(std::string name) {
		return *getDoubleArrayTag(name);
	}

	Tag* clone() {
		CompoundTag* out = new CompoundTag(name);
		for (int i = 0; i < tags.size(); i++)
			*out << (tags[i]->clone());
		return out;
	}
private:
	std::string name;
	int endIndex = 0;
};
#endif