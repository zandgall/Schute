#include "NBTEnvelope.h" 
#include "FileIO/CompoundTag.h"
#include "FileIO/FileChooser.h"

NBTEnvelope::NBTEnvelope() {
	CompoundTag t = CompoundTag();
	t.tags.push_back(new IntTag("number", 12345));
	t.tags.push_back(new StringTag("characters", "ABC æÓÑ"));

	ListTag l = ListTag("list", Tag::BYTE_ARRAY);
	l.addValue(new ByteArrayTag("", { 0x01, 0x02, 0x03, 0x04 }));
	l.addValue(new ByteArrayTag("", { 0x01, 0x02, 0x03, 0x04, 0x05 }));
	l.addValue(new ByteArrayTag("", { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 }));
	t.tags.push_back(&l);

	ListTag k = ListTag("numbers", Tag::INT);
	k.addValue(new IntTag("", 1));
	k.addValue(new IntTag("", 2));
	k.addValue(new IntTag("", 3));
	k.addValue(new IntTag("", 4));
	k.addValue(new IntTag("", 5));
	k.addValue(new IntTag("", 6));
	t.tags.push_back(&k);

	ListTag c = ListTag("nest", Tag::COMPOUND);

	CompoundTag c0 = CompoundTag("");
	c0.tags.push_back(new ByteTag("id", 'a'));
	c0.tags.push_back(new DoubleTag("pi", 3.14159265));
	c0.tags.push_back(new StringTag("symbol", "\xcf\x80"));
	
	CompoundTag c1 = CompoundTag("");
	c1.tags.push_back(new ByteTag("id", 'b'));
	c1.tags.push_back(new FloatTag("tau", 6.2831853));
	c1.tags.push_back(new StringTag("symbol", "\xcf\x84"));

	c.addValue(&c0);
	c.addValue(&c1);
	t.tags.push_back(&c);

	std::cout << "THE STORED COMPOUND: " << std::endl;
	t.getCompilation();
	writeNBT(ChooseSaveFile("Choose test NBT file", "").c_str(), t);

	CompoundTag nT = loadNBT(ChooseOpenFile("Choose test NBT file", "").c_str());
	nT.getCompilation();
}


NBTEnvelope::~NBTEnvelope()
{
}

void NBTEnvelope::tick() {

}

void NBTEnvelope::render() {
	
}

void NBTEnvelope::render_gui() {

}
