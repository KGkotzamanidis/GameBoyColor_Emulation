#ifndef MEM_H
#include <vector>
#include <cstdint>
#include <string>

#include "cartridge.h"
using namespace std;

const int Addr_loc_WRAM = 0x0000;
const int Addr_loc_ZRAM = 0x2000;
const int Addr_loc_VRAM = 0x2100;
const int Addr_loc_OAM = 0x4100;

const int Addr_loc_P1 = Addr_loc_ZRAM + 0x00;
const int Addr_loc_DIV = Addr_loc_ZRAM + 0x04;
const int Addr_loc_TIMA = Addr_loc_ZRAM + 0x05;
const int Addr_loc_TMA = Addr_loc_ZRAM + 0x06;
const int Addr_loc_TAC = Addr_loc_ZRAM + 0x07;
const int Addr_loc_LCDC = Addr_loc_ZRAM + 0x40;
const int Addr_loc_STAT = Addr_loc_ZRAM + 0x41;
const int Addr_loc_SCY = Addr_loc_ZRAM + 0x42;
const int Addr_loc_SCX = Addr_loc_ZRAM + 0x43;
const int Addr_loc_LY = Addr_loc_ZRAM + 0x44;
const int Addr_loc_LYC = Addr_loc_ZRAM + 0x45;
const int Addr_loc_DMA = Addr_loc_ZRAM + 0x46;
const int Addr_loc_BGP = Addr_loc_ZRAM + 0x47;
const int Addr_loc_OBP0 = Addr_loc_ZRAM + 0x48;
const int Addr_loc_OBP1 = Addr_loc_ZRAM + 0x49;
const int Addr_loc_WY = Addr_loc_ZRAM + 0x4A;
const int Addr_loc_WX = Addr_loc_ZRAM + 0x4B;
const int Addr_loc_IF = Addr_loc_ZRAM + 0x0F;
const int Addr_loc_IE = Addr_loc_ZRAM + 0xFF;

const int get_Size_WRAM = 0x2000;
const int get_Size_ZRAM = 0x0100;
const int get_Size_VRAM = 0x2000;
const int get_Size_OAM = 0x0100;
const int get_Total_Mem_Size = get_Size_WRAM + get_Size_ZRAM + get_Size_VRAM + get_Size_OAM;
class MEM {
	public:
		MEM();
		CARTRIDGE cartridge;
		vector<uint8_t> _SystemRAM;
		string ROM_Name;
		uint16_t _minStack = 0x0000;
		uint16_t _maxStack = 0xFFFF;
		uint8_t* getVRAM();
		uint8_t* getOAM();
		uint8_t* getWRAM();
		uint8_t* getZRAM();
		uint8_t* getMemLoc(int address);
		uint8_t getMemLocData(int address);
		uint8_t JOYPAD_Buttons;
		uint8_t JOYPAD_Arrows;
		uint8_t getJOYPAD_State();
		void cleanRAM();
		void LoadROM(string path);
		void DMATransfer();
		uint8_t readRAM(uint16_t address);
		void writeRAM(uint16_t address, uint8_t data);
		void writeZeroPage(uint16_t address, uint8_t data);
};
#endif 