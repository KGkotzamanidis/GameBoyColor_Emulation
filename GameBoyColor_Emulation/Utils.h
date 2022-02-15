#ifndef _UTILS_H
#include <cstdint>

enum class enum_byte : uint8_t {
	bit_7 = 7,
	bit_6 = 6,
	bit_5 = 5,
	bit_4 = 4,
	bit_3 = 3,
	bit_2 = 2,
	bit_1 = 1,
	bit_0 = 0,
	bit_invalid = 255
};

uint16_t setBit(uint16_t data, enum_byte bit);
uint16_t clearBit(uint16_t data, enum_byte bit);
bool isBitSet(uint16_t data, enum_byte bit);

uint16_t FLAG_ZERO = 0x80;
uint16_t FLAG_SUB = 0x40;
uint16_t FLAG_HALF_CARRY = 0x20;
uint16_t FLAG_CARRY = 0x10;

#endif
