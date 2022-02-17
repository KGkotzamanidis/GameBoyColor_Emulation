#ifndef  _CARTRIDGE_H
#pragma once
#include <vector>
using namespace std;
enum class MemBankControllType {
	DEFAULT,
	MBC1,MBC2,MBC3
};
class CARTRIDGE {
	public:
	MemBankControllType MemType;
	vector<uint8_t> ROM_Data;
	vector<uint8_t> ERAM;
	bool isRTCEnable = false;
	bool isRAMBankEnable = false;
	bool isRAMAccessEnable = false;
	uint8_t ROMBankID = 1;
	uint8_t RAMBankID = 0;
	uint8_t mode = 0;
	const uint8_t MODE_ROM = 0;
	const uint8_t MODE_RAM = 1;

	void Init(vector<uint8_t> cartridge_buffer);
	uint8_t readROM(uint16_t address);
	void writeROM(uint16_t address, uint8_t data);
};
#endif 