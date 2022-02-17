#include "memory.h"
#include <iosfwd>
#include <iostream>
#include <fstream>
#include <xutility>

MEM::MEM() {
	_SystemRAM = vector<uint8_t>(get_Total_Mem_Size);
	cleanRAM();
}
void MEM::cleanRAM() {
	for (auto& blocks : _SystemRAM) {
		blocks = 0x00;
	}
	JOYPAD_Arrows = JOYPAD_Buttons = 0xF;

}

uint8_t* MEM::getVRAM() {
	return &(_SystemRAM[Addr_loc_VRAM]);
}

uint8_t* MEM::getOAM() {
	return &(_SystemRAM[Addr_loc_OAM]);
}

uint8_t* MEM::getWRAM() {
	return &(_SystemRAM[Addr_loc_WRAM]);
}

uint8_t* MEM::getZRAM() {
	return &(_SystemRAM[Addr_loc_ZRAM]);
}

uint8_t* MEM::getMemLoc(int address) {
	return &(_SystemRAM[address]);
}

uint8_t MEM::getMemLocData(int address) {
	return _SystemRAM[address];
}

uint8_t MEM::getJOYPAD_State() {
	uint8_t JOYPADTypereq = *(getMemLoc(Addr_loc_P1));
	switch (JOYPADTypereq)
	{
		case 0x10:
			return JOYPAD_Buttons;
		case 0x20:
			return JOYPAD_Buttons;
		default:
			return 0xFF;
	}
}

void MEM::LoadROM(string path) {
	ifstream input(path, ios::binary);
	vector<uint8_t> buffer((istreambuf_iterator<char>(input)), 
								(istreambuf_iterator<char>()));
	string title = "";
	for (int i = 0x0134; i <= 0x142; i++) {
		uint8_t character = buffer[i];
		if (character == 0) {
			break;
		}
		else {
			title.push_back(tolower(character));
		}
		ROM_Name = title;
		uint8_t cartridgeMode = buffer[0x0147];

		switch (cartridgeMode) {
			case 0x03:
				cartridge.MemType = MemBankControllType::MBC1;
				break;
			case 0x06:
				cartridge.MemType = MemBankControllType::MBC2;
				break;
			case 0x13:
				cartridge.MemType = MemBankControllType::MBC3;
				break;
			default:
				cartridge.MemType = MemBankControllType::DEFAULT;
				break;
		}
		cartridge.Init(buffer);
		input.close();

	}
}
void MEM::DMATransfer() {
	uint16_t addr = (*getMemLoc(Addr_loc_DMA)) << 8;
	for (int i = 0; i < 0xA0; i++) {
		writeRAM((0xFE00 + i), readRAM(addr + i));
	}
}

void MEM::writeZeroPage(uint16_t address, uint8_t data) {
	switch (address) {
		case 0xFF00:
			getZRAM()[0x00] = (data & 0x30);
			break;
		case 0xFF04:
			getZRAM()[0x04] = 0;
			break;
		case 0xFF41:
			getZRAM()[0x41] = (data & 0xFC) | (_SystemRAM[Addr_loc_STAT] & 0x03);
			break;
		case 0xFF44:
			getZRAM()[0x44] = 0;
			break;
		case 0xFF46:
			getZRAM()[0x46] = data;
			DMATransfer();
			break;
		default:
			getZRAM()[address & 0xFF] = data;
			break;
	}
}
void MEM::writeRAM(uint16_t address, uint8_t data) {
	switch (address & 0xF000) {
		/*
		* ROM Data
		*/
		case 0x0000:
		case 0x1000:
		case 0x2000:
		case 0x3000:
		case 0x4000:
		case 0x5000:
		case 0x6000:
		case 0x7000:
			cartridge.writeROM(address, data);
			break;
		/*
		* Graphics from ROM
		*/
		case 0x8000:
		case 0x9000:
			getVRAM()[address & 0x1FFF] = data;
			break;
		case 0xA000:
		case 0xB000:
			cartridge.writeROM(address, data);
			break;
		/*
		* 8kB Ram woth Shadow
		*/
		case 0xC000:
		case 0xD000:
		case 0xE000:
			getWRAM()[address & 0x1FFF] = data;
			break;
		case 0xF000:
			switch (address & 0x0F00) {
			case 0x000:
			case 0x100:
			case 0x200:
			case 0x300:
			case 0x400:
			case 0x500:
			case 0x600:
			case 0x700:
			case 0x800:
			case 0x900:
			case 0xA00:
			case 0xB00:
			case 0xC00:
			case 0xD00:
				getWRAM()[address & 0x1FFF] = data;
				break;
			case 0xE00:
				getOAM()[address & 0xFF] = data;
				break;
			case 0xF00:
				writeZeroPage(address, data);
				break;
			}
	}
}

uint8_t MEM::readRAM(uint16_t address) {
	switch (address & 0xF000) {
		case 0x0000:
		case 0x1000:
		case 0x2000:
		case 0x3000:
		case 0x4000:
		case 0x5000:
		case 0x6000:
		case 0x7000:
			return cartridge.readROM(address);
		case 0x8000:
		case 0x9000:
			return getVRAM()[address & 0x1FFF];
		case 0xA000:
		case 0xB000:
			return cartridge.readROM(address);
		case 0xC000:
		case 0xD000:
		case 0xE000:
			return getWRAM()[address & 0x1FFF];
		case 0xF000:
			switch (address & 0x0F00) {
			case 0x000:
			case 0x100:
			case 0x200:
			case 0x300:
			case 0x400:
			case 0x500:
			case 0x600:
			case 0x700:
			case 0x800:
			case 0x900:
			case 0xA00:
			case 0xB00:
			case 0xC00:
			case 0xD00:
				return getWRAM()[address & 0x1FFF];
			case 0xE00:
				return getOAM()[address & 0xFF];
			case 0xFF:
				if (address == 0xFF00) {
					return getJOYPAD_State();
				}
				else {
					return getZRAM()[address & 0xFF];
				}
			}
		default:
			return 0xFF;
	}
}