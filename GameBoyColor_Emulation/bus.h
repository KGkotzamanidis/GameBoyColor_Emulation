#ifndef _BUS_H
#include "cpu.h"
#include "memory.h"
class BUS : public CPU{
	public:
		BUS();
		MEM mem;
		Registers reg;
	private:
};
#endif