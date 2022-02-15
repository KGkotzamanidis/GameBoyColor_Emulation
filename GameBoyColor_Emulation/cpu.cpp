#include "bus.h"
#include "cpu.h"
#include "memory.h"
#include "bus.h"
#include "motherboard.h"
#include "Utils.h"

CPU::CPU() {
	CPUClockSped = 4194304;
	numCycles = 0;
	isInterruptMasterEnable = true;
	isHalted = false;
}

void CPU::setFlag(int flag, bool state) {
	if (state) {
		sendSignal->reg.F |= flag;
	}
	else {
		sendSignal->reg.F &= ~(flag);
	}
}

void CPU::ADC(uint8_t& address, uint8_t data) {
	uint16_t carry = (sendSignal->reg.F & FLAG_CARRY) ? 1 : 0;
	uint16_t resualt = (uint16_t)address + (uint16_t)data + carry;
	setFlag(FLAG_HALF_CARRY, ((address & 0x0F) + (data & 0xF) + (uint8_t)carry) > 0x0F);
	setFlag(FLAG_CARRY, (resualt > 0xFF));
	setFlag(FLAG_ZERO, (resualt & 0xFF) == 0);
	setFlag(FLAG_SUB, false);
	address = (resualt & 0xFF);
}

void CPU::ADC_A_r8(uint8_t data) {
	ADC(sendSignal->reg.A, data);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::ADC_A_HL() {
	uint8_t val = sendSignal->mem.readRAM(sendSignal->reg.HL);
	ADC(sendSignal->reg.A, val);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::ADC_A_n8(uint8_t data) {
	ADC(sendSignal->reg.A, data);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::ADD(uint8_t& address, uint8_t data) {
	uint16_t result = (uint16_t)address + (uint16_t)data;
	setFlag(FLAG_HALF_CARRY, ((address & 0xF) + (data & 0xF)) > 0xF);
	setFlag(FLAG_CARRY, (result > 0xFF));
	setFlag(FLAG_ZERO, (result & 0xFF) == 0);
	setFlag(FLAG_SUB, false);
	address = (result & 0xFF);
}

void CPU::ADD_A_r8(uint8_t data) {
	ADD(sendSignal->reg.A, data);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::ADD_A_HL() {
	uint8_t val = sendSignal->mem.readRAM(sendSignal->reg.HL);
	ADD(sendSignal->reg.A, val);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::ADD_A_n8(uint8_t data) {
	ADD(sendSignal->reg.A, data);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::AND(uint8_t& address, uint8_t data) {
	address &= data;
	setFlag(FLAG_ZERO, (address == 0));
	setFlag(FLAG_SUB, false);
	setFlag(FLAG_HALF_CARRY, true);
	setFlag(FLAG_CARRY, false);
}

void CPU::AND_A_r8(uint8_t data) {
	AND(sendSignal->reg.A, data);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::AND_A_HL() {
	uint8_t val = sendSignal->mem.readRAM(sendSignal->reg.HL);
	AND(sendSignal->reg.A, val);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::AND_A_n8(uint8_t data) {
	AND(sendSignal->reg.A, data);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::CP(uint8_t& address, uint8_t data) {
	/*
	* Nothing for now.
	*/
}

void CPU::CP_A_r8(uint8_t data) {
	int result = sendSignal->reg.A - data;
	setFlag(FLAG_ZERO, (result == 0));
	setFlag(FLAG_SUB, true);
	setFlag(FLAG_HALF_CARRY, (((sendSignal->reg.A & 0xF) - (data & 0xF)) < 0));
	setFlag(FLAG_CARRY, (result < 0));
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::CP_A_HL() {
	uint8_t val = sendSignal->mem.readRAM(sendSignal->reg.HL);
	int result = sendSignal->reg.A - val;
	setFlag(FLAG_ZERO, (result == 0));
	setFlag(FLAG_SUB, true);
	setFlag(FLAG_HALF_CARRY, (((sendSignal->reg.A & 0xF) - (val & 0xF)) < 0));
	setFlag(FLAG_CARRY, (result < 0));
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::CP_A_n8(uint8_t data) {
	int result = sendSignal->reg.A - data;
	setFlag(FLAG_ZERO, (result == 0));
	setFlag(FLAG_SUB, true);
	setFlag(FLAG_HALF_CARRY, (((sendSignal->reg.A & 0xF) - (data & 0xF)) < 0));
	setFlag(FLAG_CARRY, (result < 0));
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::DEC_r8(uint8_t& address) {
	uint8_t result = address - 1;
	setFlag(FLAG_ZERO, (result == 0));
	setFlag(FLAG_SUB, true);
	setFlag(FLAG_HALF_CARRY, (((address & 0xF) - 1) < 0));
	address = result;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::DEC_HL() {
	uint8_t val = sendSignal->mem.readRAM(sendSignal->reg.HL);
	uint8_t result = val - 1;
	setFlag(FLAG_ZERO, (result == 0));
	setFlag(FLAG_SUB, true);
	setFlag(FLAG_HALF_CARRY, (((val & 0xF) - 1) < 0));
	sendSignal->mem.writeRAM(sendSignal->reg.HL, result);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 12;
}

void CPU::INC_r8(uint8_t& address) {
	uint8_t result = address + 1;
	setFlag(FLAG_ZERO, (result == 0));
	setFlag(FLAG_SUB, false);
	setFlag(FLAG_HALF_CARRY, ((((address & 0xF) + 1) & 0x10) != 0));
	address = result;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::INC_HL() {
	uint8_t val = sendSignal->mem.readRAM(sendSignal->reg.HL);
	uint8_t result = val + 1;
	setFlag(FLAG_ZERO, (result == 0));
	setFlag(FLAG_SUB, false);
	setFlag(FLAG_HALF_CARRY, ((((val & 0xF) + 1) & 0x10) != 0));
	sendSignal->mem.writeRAM(sendSignal->reg.HL, result);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 12;
}

void CPU::OR(uint8_t& address, uint8_t data) {
	address |= data;
	setFlag(FLAG_ZERO, (address == 0));
	setFlag(FLAG_SUB, false);
	setFlag(FLAG_HALF_CARRY, false);
	setFlag(FLAG_CARRY, false);
}

void CPU::OR_A_r8(uint8_t data) {
	OR(sendSignal->reg.A, data);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::OR_A_HL() {
	OR(sendSignal->reg.A, sendSignal->mem.readRAM(sendSignal->reg.HL));
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::OR_A_n8(uint8_t data) {
	OR(sendSignal->reg.A, data);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 4;
}

void CPU::SBC(uint8_t& address, uint8_t data) {
	uint16_t carry = (sendSignal->reg.F & FLAG_CARRY) ? 1 : 0;
	int16_t result = (int16_t)address - (int16_t)data - carry;
	int16_t a_address = (int16_t)address;
	int16_t a_data = (int16_t)data;
	setFlag(FLAG_HALF_CARRY, (((a_address & 0xF) - (a_data & 0xF) - carry) < 0));
	setFlag(FLAG_CARRY, result < 0);
	setFlag(FLAG_SUB, true);
	setFlag(FLAG_ZERO, (result & 0xFF) == 0);
	address = (uint8_t)(result & 0xFF);
}

void CPU::SUB_A_r8(uint8_t data) {
	SBC(sendSignal->reg.A, data);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::SBC_A_HL() {
	uint8_t val = sendSignal->mem.readRAM(sendSignal->reg.HL);
	SBC(sendSignal->reg.A, val);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::SBC_A_n8(uint8_t data) {
	SBC(sendSignal->reg.A, data);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::SUB(uint8_t& address, uint8_t data) {
	int16_t resualt = (int16_t)address - (int16_t)data;
	int16_t a_address = (int16_t)address;
	int16_t a_data = (int16_t)data;
	setFlag(FLAG_HALF_CARRY, (((a_address & 0xF) - (a_data & 0xF)) < 0));
	setFlag(FLAG_CARRY, resualt < 0);
	setFlag(FLAG_SUB, true);
	setFlag(FLAG_ZERO, (resualt & 0xFF) == 0);
	address = (uint8_t)(resualt & 0xFF);
}

void CPU::SUB_A_r8(uint8_t data) {
	SUB(sendSignal->reg.A, data);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::SUB_A_HL() {
	uint8_t val = sendSignal->mem.readRAM(sendSignal->reg.HL);
	SUB(sendSignal->reg.A, val);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::SUB_A_n8(uint8_t data) {
	SUB(sendSignal->reg.A, data);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::XOR(uint8_t& address, uint8_t data) {
	address ^= data;
	setFlag(FLAG_ZERO, (address == 0));
	setFlag(FLAG_SUB, false);
	setFlag(FLAG_HALF_CARRY, false);
	setFlag(FLAG_CARRY, false);
}

void CPU::XOR_A_r8(uint8_t data) {
	XOR(sendSignal->reg.A, data);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::XOR_A_HL() {
	XOR(sendSignal->reg.A, sendSignal->mem.readRAM(sendSignal->reg.HL));
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::XOR_A_n8(uint8_t data) {
	XOR(sendSignal->reg.A, data);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::ADD_HL_r16(uint16_t data) {
	uint16_t resualt = sendSignal->reg.HL + data;
	setFlag(FLAG_SUB, false);
	setFlag(FLAG_HALF_CARRY, ((((sendSignal->reg.HL & 0xFFF) + (data & 0xFFF)) & 0x1000) != 0));
	setFlag(FLAG_CARRY, (resualt > 0xFFFF));
	sendSignal->reg.HL = resualt;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::DEC_r16(uint16_t& address) {
	address--;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::INC_r16(uint16_t& address) {
	address = address + 1;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::BIT_u3_r8(uint8_t data, uint8_t bit) {
	setFlag(FLAG_ZERO, (((1 << bit) & ~data) != 0));
	setFlag(FLAG_HALF_CARRY, true);
	setFlag(FLAG_SUB, false);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::BIT_u3_HL(uint8_t bit) {
	uint8_t value = sendSignal->mem.readRAM(sendSignal->reg.HL);
	setFlag(FLAG_ZERO, (((1 << bit) & ~value) != 0));
	setFlag(FLAG_HALF_CARRY, true);
	setFlag(FLAG_SUB, false);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 12;
}

void CPU::RES_u3_r8(uint8_t& address, uint8_t bit) {
	address = (~(1 << bit) & address);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::RES_u3_HL(uint8_t bit) {
	uint8_t value = sendSignal->mem.readRAM(sendSignal->reg.HL);
	value = (~(1 << bit) & value);
	sendSignal->mem.writeRAM(sendSignal->reg.HL, value);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 16;
}

void CPU::SET_u3_r8(uint8_t& address, uint8_t bit) {
	address = (address | (1 << bit));
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::SET_u3_HL(uint8_t bit) {
	uint8_t value = sendSignal->mem.readRAM(sendSignal->reg.HL);
	value = (value | (1 << bit));
	sendSignal->mem.writeRAM(sendSignal->reg.HL, value);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 16;
}

void CPU::SWAP_r8(uint8_t& address) {
	uint8_t first = address >> 4;
	uint8_t second = address << 4;
	uint8_t swapped = first | second;
	address = swapped;
	setFlag(FLAG_CARRY, false);
	setFlag(FLAG_HALF_CARRY, false);
	setFlag(FLAG_SUB, false);
	setFlag(FLAG_ZERO, (address == 0));
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::SWAP_HL() {
	uint8_t value = sendSignal->mem.readRAM(sendSignal->reg.HL);
	uint8_t first = value >> 4;
	uint8_t second = value << 4;
	uint8_t swapped = first | second;
	value = swapped;
	setFlag(FLAG_CARRY, false);
	setFlag(FLAG_HALF_CARRY, false);
	setFlag(FLAG_SUB, false);
	setFlag(FLAG_ZERO, (value == 0));
	sendSignal->mem.writeRAM(sendSignal->reg.HL, value);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 16;
}

void CPU::RL_r8(uint8_t& address) {
	RL(address, true, true);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::RL_HL() {
	RL(sendSignal->reg.HL, true);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 16;
}

void CPU::RLA() {
	RL(sendSignal->reg.A, true);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::RLC_r8(uint8_t& address) {
	RL(address, false, true);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::RLC_HL() {
	RL(sendSignal->reg.HL, false);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 16;
}

void CPU::RL(uint16_t address, bool carry) {
	uint8_t value = sendSignal->mem.readRAM(address);
	RL(value, carry, true);
	sendSignal->mem.writeRAM(address, value);
}

void CPU::RL(uint8_t& address, bool carry, bool zero_flag = false) {
	int bit7 = ((address & 0x80) != 0);
	address = address << 1;
	address |= (carry) ? ((sendSignal->reg.F & FLAG_CARRY) != 0) : bit7;
	setFlag(FLAG_ZERO, ((zero_flag) ? (address == 0) : false));
	setFlag(FLAG_SUB, false);
	setFlag(FLAG_HALF_CARRY, false);
	setFlag(FLAG_CARRY, (bit7 != 0));
}

void CPU::RLCA() {
	RL(sendSignal->reg.A, false);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::RR_r8(uint8_t& address) {
	RR(address, true, true);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::RR_HL() {
	RR(sendSignal->reg.A, true);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::RR_A() {
	RR(sendSignal->reg.A, true);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::RRC_r8(uint8_t& address) {
	RR(address, false, true);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::RRC_HL() {
	RR(sendSignal->reg.HL, false);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 16;
}

void CPU::RR(uint16_t address, bool carry) {
	uint8_t value = sendSignal->mem.readRAM(address);
	RR(value, carry, true);
	sendSignal->mem.writeRAM(address, value);
}

void CPU::RR(uint8_t& address, bool carry, bool zero_flag = false) {
	int bit1 = ((address & 0x1) != 0);
	address = address >> 1;
	address |= (carry) ? (((sendSignal->reg.F & FLAG_CARRY) != 0) << 7) : (bit1 << 7);
	setFlag(FLAG_ZERO, ((zero_flag) ? (address == 0) : false));
	setFlag(FLAG_SUB, false);
	setFlag(FLAG_HALF_CARRY, false);
	setFlag(FLAG_CARRY, (bit1 != 0));
}

void CPU::RRC_A() {
	RR(sendSignal->reg.A, false);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::SR(uint8_t& address, bool include_top_bit) {
	uint8_t resualt;
	bool top_bit_set = isBitSet(address, enum_byte::bit_7);
	if (include_top_bit) {
		resualt = (top_bit_set) ? ((address >> 1) | 0x80) : (address >> 1);
	}
	else {
		resualt = address >> 1;
	}

	setFlag(FLAG_CARRY, (address & 0x01) != 0);
	setFlag(FLAG_HALF_CARRY, false);
	setFlag(FLAG_SUB, false);
	setFlag(FLAG_ZERO, (resualt == 0));

	address = resualt;
}

void CPU::SRA_r8(uint8_t& address) {
	SR(address, true);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::SRA_HL() {
	uint8_t data = sendSignal->mem.readRAM(sendSignal->reg.HL);
	SR(data, true);
	sendSignal->mem.writeRAM(sendSignal->reg.HL, data);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 16;
}

void CPU::SRL_r8(uint8_t& address) {
	SR(address, false);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::SRL_HL() {
	uint8_t data = sendSignal->mem.readRAM(sendSignal->reg.HL);
	SR(data, false);
	sendSignal->mem.writeRAM(sendSignal->reg.HL, data);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 16;
}

void CPU::LD_r8_r8(uint8_t& address, uint8_t data) {
	address = data;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::LD_r8_n8(uint8_t& address, uint8_t data) {
	address = data;
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
}

void CPU::LD_r16_n16(uint16_t& address, uint16_t data) {
	address = data;
	sendSignal->reg.PC += 3;
	sendSignal->numCycles += 12;
}

void CPU::LD_HL_r8(uint8_t data) {
	sendSignal->mem.writeRAM(sendSignal->reg.HL, data);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::LD_HL_n8(uint8_t data) {
	sendSignal->mem.writeRAM(sendSignal->reg.HL, data);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 12;
}

void CPU::LD_r8_HL(uint8_t& address) {
	address = sendSignal->mem.readRAM(sendSignal->reg.HL);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::LD_r16_A(uint16_t address) {
	sendSignal->mem.writeRAM(address, sendSignal->reg.A);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::LD_n16_A(uint16_t address) {
	sendSignal->mem.writeRAM(address, sendSignal->reg.A);
	sendSignal->reg.PC += 3;
	sendSignal->numCycles += 16;
}

void CPU::LDH_n16_A(uint16_t address) {
	uint16_t addr = (uint16_t)(0xFF00 + address);
	sendSignal->mem.writeRAM(addr, sendSignal->reg.A);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 12;
}

void CPU::LDH_C_A() {
	uint16_t addr = (uint16_t)(0xFF00 + sendSignal->reg.C);
	sendSignal->mem.writeRAM(addr, sendSignal->reg.A);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::LD_A_r16(uint16_t address) {
	sendSignal->reg.A = sendSignal->mem.readRAM(address);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::LD_A_n16(uint16_t address) {
	sendSignal->reg.A = sendSignal->mem.readRAM(address);
	sendSignal->reg.PC += 3;
	sendSignal->numCycles += 16;
}

void CPU::LDH_A_n16(uint16_t address) {
	uint16_t addr = (uint16_t)(0xFF00 + address);
	sendSignal->reg.A = sendSignal->mem.readRAM(addr);
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 12;
}

void CPU::LDH_A_C() {
	uint16_t addr = (uint16_t)(0xFF00 + sendSignal->reg.C);
	sendSignal->reg.A = sendSignal->mem.readRAM(addr);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::LD_HLI_A() {
	sendSignal->mem.writeRAM(sendSignal->reg.HL, sendSignal->reg.A);
	sendSignal->reg.HL++;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::LD_HLD_A() {
	sendSignal->mem.writeRAM(sendSignal->reg.HL, sendSignal->reg.A);
	sendSignal->reg.HL--;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::LD_A_HLI() {
	sendSignal->reg.A = sendSignal->mem.readRAM(sendSignal->reg.HL);
	sendSignal->reg.HL++;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::LD_A_HLD() {
	sendSignal->reg.A = sendSignal->mem.readRAM(sendSignal->reg.HL);
	sendSignal->reg.HL--;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::CALL(uint16_t address) {
	sendSignal->reg.SP--;
	sendSignal->mem.writeRAM(sendSignal->reg.SP, sendSignal->reg.PC >> 8 & 0xFF);
	sendSignal->reg.SP--;
	sendSignal->mem.writeRAM(sendSignal->reg.SP, sendSignal->reg.PC & 0xFF);
	JP(address);
	sendSignal->numCycles += 12;
}

void CPU::CALL_n16(uint16_t address) {
	sendSignal->reg.PC += 3;
	sendSignal->numCycles += 12;
	CALL(address);
}

void CPU::CALLNZ_n16(uint16_t address) {
	sendSignal->reg.PC += 3;
	sendSignal->numCycles += 12;

	if ((sendSignal->reg.F & FLAG_ZERO) == 0) {
		CALL(address);
	}
}

void CPU::CALLZ_n16(uint16_t address) {
	sendSignal->reg.PC += 3;
	sendSignal->numCycles += 12;

	if ((sendSignal->reg.F & FLAG_ZERO) != 0) {
		CALL(address);
	}
}

void CPU::CALLNC_n16(uint16_t address) {
	sendSignal->reg.PC += 3;
	sendSignal->numCycles += 12;

	if ((sendSignal->reg.F & FLAG_CARRY) == 0) {
		CALL(address);
	}
}

void CPU::CALLC_n16(uint16_t address) {
	sendSignal->reg.PC += 3;
	sendSignal->numCycles += 12;

	if ((sendSignal->reg.F & FLAG_CARRY) != 0) {
		CALL(address);
	}
}

void CPU::JP(uint16_t address) {
	sendSignal->reg.PC = address;
	sendSignal->numCycles += 4;
}

void CPU::JP_HL(){
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
	sendSignal->reg.PC = sendSignal->reg.HL;
}

void CPU::JP_n16(uint16_t address) {
	sendSignal->reg.PC += 3;
	sendSignal->numCycles += 12;
	JP(address);
}

void CPU::JPNZ_n16(uint16_t address) {
	sendSignal->reg.PC += 3;
	sendSignal->numCycles += 12;

	if ((sendSignal->reg.F & FLAG_ZERO) == 0) {
		JP(address);
	}
}

void CPU::JPZ_n16(uint16_t address) {
	sendSignal->reg.PC += 3;
	sendSignal->numCycles += 12;

	if ((sendSignal->reg.F & FLAG_ZERO) != 0) {
		JP(address);
	}
}

void CPU::JPNC_n16(uint16_t address) {
	sendSignal->reg.PC += 3;
	sendSignal->numCycles += 12;

	if ((sendSignal->reg.F & FLAG_CARRY) == 0) {
		JP(address);
	}
}

void CPU::JPC_n16(uint16_t address) {
	sendSignal->reg.PC += 3;
	sendSignal->numCycles += 12;

	if ((sendSignal->reg.F & FLAG_CARRY) != 0) {
		JP(address);
	}
}

void CPU::JP_CC_n16(uint8_t condition_code, uint16_t address) {

}

void CPU::JR(uint8_t data) {
	int8_t signed_val = ((int8_t)(data));
	sendSignal->reg.PC += signed_val;
	sendSignal->numCycles += 4;
}

void CPU::JR_n8(uint8_t data) {
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;
	JR(data);
}

void CPU::JRNZ_n8(uint8_t data) {
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;

	if ((sendSignal->reg.F & FLAG_ZERO) == 0) {
		JR(data);
	}
}

void CPU::JRZ_n8(uint8_t data) {
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;

	if ((sendSignal->reg.F & FLAG_ZERO) != 0) {
		JR(data);
	}
}

void CPU::JRNC_n8(uint8_t data) {
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;

	if ((sendSignal->reg.F & FLAG_CARRY) == 0) {
		JR(data);
	}
}

void CPU::JRC_n8(uint8_t data) {
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 8;

	if ((sendSignal->reg.F & FLAG_CARRY) != 0) {
		JR(data);
	}
}

void CPU::JR_e8(uint8_t offset) {

}

void CPU::JR_CC_e8(uint8_t condition_code, uint16_t address) {

}

void CPU::RET() {
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
	RET_Impl();
}

void CPU::RETI() {
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
	sendSignal->isInterruptMasterEnable = true;
	RET_Impl();
}

void CPU::RETNZ() {
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;

	if ((sendSignal->reg.F & FLAG_ZERO) == 0) {
		RET_Impl();
		sendSignal->numCycles += 8;
	}
}

void CPU::RETZ() {
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;

	if ((sendSignal->reg.F & FLAG_ZERO) != 0) {
		RET_Impl();
		sendSignal->numCycles += 8;
	}
}

void CPU::RETNC() {
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;

	if ((sendSignal->reg.F & FLAG_CARRY) == 0) {
		RET_Impl();
		sendSignal->numCycles += 8;
	}
}

void CPU::RETC() {
	sendSignal->reg.F += 1;
	sendSignal->numCycles += 8;

	if ((sendSignal->reg.F & FLAG_CARRY) != 0) {
		RET_Impl();
		sendSignal->numCycles += 8;
	}
}

void CPU::RET_CC(uint8_t condition_code) {

}

void CPU::RET_Impl() {
	uint8_t low = sendSignal->mem.readRAM(sendSignal->reg.SP++);
	uint8_t high = sendSignal->mem.readRAM(sendSignal->reg.SP++);
	sendSignal->reg.PC = (high << 8 & 0xFF00) | (low & 0xFF);
	sendSignal->numCycles += 12;
}

void CPU::RST_VEC(uint16_t address) {
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 16;
	sendSignal->reg.SP--;
	sendSignal->mem.writeRAM(sendSignal->reg.SP, (sendSignal->reg.PC >> 8 & 0xFF));
	sendSignal->reg.SP--;
	sendSignal->mem.writeRAM(sendSignal->reg.SP, (sendSignal->reg.PC & 0xFF));
	sendSignal->reg.PC = address;
}

void CPU::ADD_HL_SP() {
	int result = sendSignal->reg.HL + sendSignal->reg.SP;
	setFlag(FLAG_SUB, false);
	setFlag(FLAG_HALF_CARRY, ((((sendSignal->reg.HL & 0xFF) + (sendSignal->reg.SP & 0xFFF)) & 0x1000) != 0));
	setFlag(FLAG_CARRY, (result > 0xFFFF));
	sendSignal->reg.HL = result & 0xFFFF;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::ADD_SP_e8(uint8_t offset) {
	int16_t val_signed = (int16_t)(int8_t)offset;
	int result = (uint16_t)((int16_t)sendSignal->reg.SP + val_signed);
	setFlag(FLAG_CARRY, (result & 0xFF) < (sendSignal->reg.SP + val_signed));
	setFlag(FLAG_HALF_CARRY, (result & 0xFF) < (sendSignal->reg.SP & 0xF));
	setFlag(FLAG_SUB, false);
	setFlag(FLAG_ZERO, false);
	sendSignal->reg.SP = result & 0xFFFF;
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 16;
}

void CPU::DEC_SP() {
	sendSignal->reg.SP--;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::INC_SP() {
	sendSignal->reg.SP++;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::LD_SP_n16(uint16_t data) {
	sendSignal->reg.SP = data;
	sendSignal->reg.PC += 3;
	sendSignal->numCycles += 12;
}

void CPU::LD_n16_SP(uint16_t address) {
	uint8_t lsb = sendSignal->reg.SP & 0xFF;
	uint8_t msb = sendSignal->reg.SP >> 8 & 0xFF;
	sendSignal->mem.writeRAM(address, lsb);
	address++;
	sendSignal->mem.writeRAM(address, msb);
	sendSignal->reg.PC += 3;
	sendSignal->numCycles += 20;
}

void CPU::LD_HL_SP_e8(uint8_t offset) {
	int16_t signed_val = (int16_t)(int8_t)offset;
	uint16_t result = (uint16_t)((int16_t)sendSignal->reg.SP + signed_val);
	setFlag(FLAG_CARRY, (result & 0xFF) < (sendSignal->reg.SP & 0xFF));
	setFlag(FLAG_HALF_CARRY, (result & 0xF) < (sendSignal->reg.SP & 0xFF));
	setFlag(FLAG_ZERO, false);
	setFlag(FLAG_SUB, false);
	sendSignal->reg.HL = result;
	sendSignal->reg.PC += 2;
	sendSignal->numCycles += 12;
}

void CPU::LD_SP_HL() {
	sendSignal->reg.SP = sendSignal->reg.HL;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 8;
}

void CPU::POP_AF() {
	uint8_t low = sendSignal->mem.readRAM(sendSignal->reg.SP++);
	uint8_t high = sendSignal->mem.readRAM(sendSignal->reg.SP++);
	sendSignal->reg.AF = high << 8 | low;
	sendSignal->reg.F &= 0xF0;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 12;
}

void CPU::POP_r16(uint16_t& data) {
	uint8_t low = sendSignal->mem.readRAM(sendSignal->reg.SP++);
	uint8_t high = sendSignal->mem.readRAM(sendSignal->reg.SP++);
	data = high << 8 | low;
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 12;
}

void CPU::PUSH_AF() {

}

void CPU::PUSH_r16(uint16_t data) {
	sendSignal->reg.SP--;
	sendSignal->mem.writeRAM(sendSignal->reg.SP, data >> 8 & 0xFF);
	sendSignal->reg.SP--;
	sendSignal->mem.writeRAM(sendSignal->reg.SP, data & 0xFF);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 16;
}

void CPU::CCF() {
	setFlag(FLAG_SUB, false);
	setFlag(FLAG_HALF_CARRY, false);
	setFlag(FLAG_CARRY, ((sendSignal->reg.F & FLAG_CARRY) ? 1 : 0) ^ 1);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::CPL() {
	sendSignal->reg.A = ~sendSignal->reg.A;
	setFlag(FLAG_HALF_CARRY, true);
	setFlag(FLAG_SUB, true);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::DAA() {
	uint8_t high = sendSignal->reg.A >> 4 & 0xF;
	uint8_t low = sendSignal->reg.A & 0xF;
	bool add = ((sendSignal->reg.F & FLAG_SUB) == 0);
	bool carry = ((sendSignal->reg.F & FLAG_CARRY) != 0);
	bool half_carry = ((sendSignal->reg.F & FLAG_HALF_CARRY) != 0);
	uint16_t result = (uint16_t)sendSignal->reg.A;
	uint16_t correction = (carry) ? 0x60 : 0x00;

	if (half_carry || (add) && ((result & 0x0F) > 9)) {
		correction |= 0x06;
	}
	if (carry || (add) && (result > 0x99)) {
		correction |= 0x60;
	}
	if (add) {
		result += correction;
	}
	else {
		result -= correction;
	}
	if (((correction << 2) & 0x100) != 0) {
		setFlag(FLAG_CARRY, true);
	}
	setFlag(FLAG_HALF_CARRY, false);
	sendSignal->reg.A = (uint8_t)(result & 0xFF);
	setFlag(FLAG_ZERO, sendSignal->reg.A == 0);
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::DI() {
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
	sendSignal->isInterruptMasterEnable = false;
}

void CPU::EI() {
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
	sendSignal->isInterruptMasterEnable = true;
}

void CPU::HALT() {
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
	sendSignal->isHalted = true;
	sendSignal->reg.PC--;
}

void CPU::NOP() {
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}

void CPU::SCF() {
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
	setFlag(FLAG_SUB, false);
	setFlag(FLAG_HALF_CARRY, false);
	setFlag(FLAG_CARRY, true);
}

void CPU::STOP() {
	sendSignal->reg.PC += 1;
	sendSignal->numCycles += 4;
}