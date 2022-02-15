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