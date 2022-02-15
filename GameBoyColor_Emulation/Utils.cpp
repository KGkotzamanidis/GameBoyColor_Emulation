#include "Utils.h"

uint16_t Utils::setBit(uint16_t data, enum_byte bit) {
	return data | (1 << (uint8_t)bit);
}

uint16_t Utils::clearBit(uint16_t data, enum_byte bit) {
	return data & (~(1 << (uint8_t)bit));
}

bool Utils::isBitSet(uint16_t data, enum_byte bit) {
	return ((data >> (uint8_t)bit) & 1) ? true : false;
}