#include "memory.h"

void MEM::cleanRAM() {
	for (auto& blocks : _SystenRAM) {
		blocks = 0x00;
	}
}

void MEM::writeRAM(uint16_t address, uint8_t data) {
	switch (address & 0xF000) {

	}
}

uint8_t MEM::readRAM(uint16_t address) {
	switch (address & 0xF000) {

	}
}