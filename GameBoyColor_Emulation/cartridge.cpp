#include "cartridge.h"
#include "Utils.h"
#include <iostream>

void CARTRIDGE::Init(vector<uint8_t> cartridge_buffer) {
	ROM_Data = cartridge_buffer;
	ERAM = vector<uint8_t>(0x8000);
}

uint8_t CARTRIDGE::readROM(uint16_t address) {
	switch (MemType) {
	case MemBankControllType::DEFAULT:
		if (address >= 0x0000 && address <= 0x7FFF) {
			return ROM_Data[address];
		}
		else if (address >= 0xA000 && address <= 0xBFFF) {
			return ERAM[address & 0x1FFF];
		}
		else {
			return 0x00;
		}
		break;
		case MemBankControllType::MBC1:
			if (address >= 0x0000 && address <= 0x3FFF) {
				return ROM_Data[address];
			}
			else if (address >= 0x4000 && address <= 0x7FFF) {
				uint8_t temp_id = ROMBankID;
				int offset = address - 0x4000;
				int lookup = (temp_id * 0x4000) + offset;
				return ROM_Data[lookup];
			}
			else if (address >= 0xA000 && address <= 0xBFFF) {
				if (isRAMAccessEnable == false) {
					return 0xFF;
				}else{
					uint8_t temp_id = (isRAMBankEnable) ? RAMBankID : 0x00;
					int offset = address - 0xA000;
					int lookup = (temp_id * 0x2000) + offset;
					return ERAM[lookup];
				}
			}
			break;
		case MemBankControllType::MBC2:
			break;
		case MemBankControllType::MBC3:
			if (address >= 0x0000 && address <= 0x3FFF) {
				return ROM_Data[address];
			}
			else if (address >= 0x4000 && address <= 0x7FFF) {
				int offset = address - 0x4000;
				int lookup = (ROMBankID * 0x4000) + offset;
				return ROM_Data[lookup];
			}
			else if (address >= 0xA000 && address <= 0xBFFF) {
				if (isRTCEnable) {
					return 0x00;
				}
				if (isRAMAccessEnable == false) {
					return 0xFF;
				}
				int offset = address - 0xA000;
				int lookup = (RAMBankID * 0x2000) + offset;
				return ERAM[lookup];
			}
			break;
	}
	return 0;
}

void CARTRIDGE::writeROM(uint16_t address, uint8_t data) {
	switch (MemType) {
		case MemBankControllType::DEFAULT:
			if (address >= 0xA000 && address <= 0xBFFF) {
				ERAM[address & 0x1FFF] = data;
			}
			break;
		case MemBankControllType::MBC1:
			if (address >= 0x0000 && address <= 0x1FFF) {
				isRAMAccessEnable = ((data & 0x0A) > 0) ? true : false;
			}
			else if (address >= 0x2000 && address <= 0x3FFF) {
				uint8_t bank_id = data & 0x1F;
				ROMBankID = (ROMBankID & 0xE0) | bank_id;
				switch (ROMBankID) {
					case 0x00:
					case 0x20:
					case 0x40:
					case 0x60:
						ROMBankID++;
						break;
				}
			}
			else if (address >= 0x4000 && address <= 0x5FFF) {
				uint8_t bank_id = data & 0x03;
				if (isRAMBankEnable) {
					RAMBankID = bank_id;
				}
				else {
					ROMBankID = ROMBankID | (bank_id << 5);
					switch (ROMBankID) {
						case 0x00:
						case 0x20:
						case 0x40:
						case 0x60:
							ROMBankID++;
							break;
					}
				}
			}
			else if (address >= 0x6000 && address <= 0x7FFF) {
				isRAMBankEnable = isBitSet(data, (enum_byte)0);
			}
			else if (address >= 0xA000 && address <= 0xBFFF) {
				if (isRAMAccessEnable) {
					int offset = address - 0xA000;
					int lookup = (RAMBankID * 0x2000) + offset;
					ERAM[lookup] = data;
				}
			}
			break;
		case MemBankControllType::MBC2:
			break;
		case MemBankControllType::MBC3:
			if (address >= 0x0000 && address <= 0x1FFF) {
				if ((data & 0x0A) > 0) {
					isRAMAccessEnable = true;
					isRTCEnable = true;
				}
				else {
					isRAMAccessEnable = false;
					isRTCEnable = false;
				}
			}
			else if (address >= 0x4000 && address <= 0x5FFF) {
				if (data <= 0x3) {
					isRTCEnable = false;
					RAMBankID = data;
				}
				else if (data >= 0x08 && data <= 0x0C) {
					isRTCEnable = true;
				}
			}
			else if (address >= 0x6000 && address <= 0x7FFF) {
				/*
				* Nothing.
				*/
			}
			else if (address >= 0xA000 && address <= 0xBFFF) {
				if (!isRTCEnable) {
					if (!isRAMAccessEnable) {
						return;
					}
					int offset = address - 0xA000;
					int lookup = (RAMBankID * 0x2000) + offset;
					ERAM[lookup] = data;
				}
				else {
					/*
					* Nothing
					*/
				}
			}
			break;
	}
}