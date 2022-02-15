#ifndef MEM_H
#include <array>
#include <cstdint>

using namespace std;
class MEM {
	public:
	array<uint16_t, 64 * 1024> _SystenRAM;
	uint16_t _minStack = 0x0000;
	uint16_t _maxStack = 0xFFFF;

	void cleanRAM();
	uint8_t readRAM(uint16_t address);
	void writeRAM(uint16_t address, uint8_t data);
};
#endif 