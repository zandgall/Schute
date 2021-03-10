#pragma once
#include <Windows.h>
#include <vector>
#include <string>
class Tag {
public:
	static enum {
		END = 0,
		BYTE = 1,
		SHORT = 2,
		INT = 3,
		LONG = 4,
		FLOAT = 5,
		DOUBLE = 6,
		BYTE_ARRAY = 7,
		STRING = 8,
		LIST = 9,
		COMPOUND = 10,
		INT_ARRAY = 11,
		LONG_ARRAY = 12,
		FLOAT_ARRAY = 13,
		DOUBLE_ARRAY = 14
	};
	Tag() {}
	virtual ~Tag() {};
	virtual void load(unsigned char* bytes, int index) = 0;
	virtual void setName(std::string name) = 0;
	virtual int next(unsigned char* bytes, int index) = 0;
	virtual std::vector<unsigned char> getBytesValue() = 0;
	virtual std::vector<unsigned char> outBytes() = 0;
	virtual const char* getName() = 0;
	virtual unsigned int id() = 0;
	virtual Tag* clone() = 0;
};

