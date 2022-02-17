#ifndef _BUS_H
#include "cpu.h"
#include "memory.h"
class BUS : public CPU{
	public:
		MEM mem;
		Registers reg;
};
#endif