#pragma once
#include "Envelope.h"
class NBTEnvelope : public Envelope
{
public:
	NBTEnvelope();
	~NBTEnvelope();
	void tick();
	void render();
	void render_gui();
};

