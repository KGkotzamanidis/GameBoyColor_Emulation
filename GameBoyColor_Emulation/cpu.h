#ifndef _CPU_H
#include <cstdint>

#include "motherboard.h"
struct Registers {
	struct {
		union {
			struct {
				uint8_t A;
				uint8_t F;
			};
			uint16_t AF;
		};
	};
	struct {
		union {
			struct {
				uint8_t B;
				uint8_t C;
			};
			uint16_t BC;
		};
	};
	struct {
		union {
			struct {
				uint8_t D;
				uint8_t E;
			};
			uint16_t DE;
		};
	};
	struct {
		union {
			struct {
				uint8_t H;
				uint8_t L;
			};
			uint16_t HL;
		};
	};
	uint16_t SP = 0;
	uint16_t PC = 0x100;

	void resetCPU() {
		A = 0;
		B = 0;
		C = 0;
		D = 0;
		E = 0;
		H = 0;
		L = 0;
		F = 0;
		SP = 0;
		PC = 0x100;
	}
};
class CPU : public MOTHERBOARD{
	public:
		int CPUClockSped;
		int numCycles;
		bool isInterruptMasterEnable, isHalted;
		CPU();
		void ParseOpcode(uint8_t opCode);
		void ParseBitOperation(uint8_t op);

	private:
		void setFlag(int flag, bool state);

		uint8_t readCPU(uint16_t address);
		void writeCPU(uint16_t address, uint8_t data);

		/* Gameboy Color CPU Instructions:
		https ://rgbds.gbdev.io/docs/v0.5.2/gbz80.7 
		*/

		/* 8-bit Arithmetic and Logical Instructions (ALU).
		* r8: Any of the 8-bit register(A,B,C,D,E,H,L).
		* n8: 8-bit integer constant.
		*/
		void ADC(uint8_t& address, uint8_t data);
		void ADC_A_r8(uint8_t data);
		void ADC_A_HL();
		void ADC_A_n8(uint8_t data);

		void ADD(uint8_t& address, uint8_t data);
		void ADD_A_r8(uint8_t data);
		void ADD_A_HL();
		void ADD_A_n8(uint8_t data);

		void AND(uint8_t& address, uint8_t data);
		void AND_A_r8(uint8_t data);
		void AND_A_HL();
		void AND_A_n8(uint8_t data);

		void CP(uint8_t& address, uint8_t data);
		void CP_A_r8(uint8_t data);
		void CP_A_HL();
		void CP_A_n8(uint8_t data);

		void DEC_r8(uint8_t& address);
		void DEC_HL();

		void INC_r8(uint8_t& address);
		void INC_HL();

		void OR(uint8_t& address, uint8_t data);
		void OR_A_r8(uint8_t data);
		void OR_A_HL();
		void OR_A_n8(uint8_t data);

		void SBC(uint8_t& address, uint8_t data);
		void SBC_A_r8(uint8_t data);
		void SBC_A_HL();
		void SBC_A_n8(uint8_t data);

		void SUB(uint8_t& address, uint8_t data);
		void SUB_A_r8(uint8_t data);
		void SUB_A_HL();
		void SUB_A_n8(uint8_t data);

		void XOR(uint8_t& address, uint8_t data);
		void XOR_A_r8(uint8_t data);
		void XOR_A_HL();
		void XOR_A_n8(uint8_t data);

		/* 16-bit Arithmetic and Logical Instructions (ALU).
		* r16: Any of the 16-bit register(AF,BC,DE,HL).
		* n16: 16-bit integer constant.
		*/
		void ADD_HL_r16(uint16_t data);
		void DEC_r16(uint16_t& address);
		void INC_r16(uint16_t& address);

		/* Bit operations Instructions.
		* r8: Any of the 8-bit register(A,B,C,D,E,H,L).
		* u3: 3-bit unsigned integer constant (0 to 7).
		*/
		void BIT_u3_r8(uint8_t data, uint8_t bit);
		void BIT_u3_HL(uint8_t bit);

		void RES_u3_r8(uint8_t& address, uint8_t bit);
		void RES_u3_HL(uint8_t bit);

		void SET_u3_r8(uint8_t& address, uint8_t bit);
		void SET_u3_HL(uint8_t bit);

		void SWAP_r8(uint8_t& address);
		void SWAP_HL();

		/* Bit shift Instructions.
		* r8: Any of the 8-bit register(A,B,C,D,E,H,L).
		*/
		void RL_r8(uint8_t& address);
		void RL_HL();
		void RLA();

		void RLC_r8(uint8_t& address);
		void RLC_HL();
		void RL(uint8_t& address, bool carry, bool zero_flag = false);
		void RL(uint16_t address, bool carry);
		void RLCA();

		void RR_r8(uint8_t& address);
		void RR_HL();
		void RR_A();

		void RRC_r8(uint8_t& address);
		void RRC_HL();
		void RR(uint8_t& address, bool carry, bool zero_flag = false);
		void RR(uint16_t address, bool carry);
		void RRC_A();

		void SR(uint8_t& address, bool include_top_bit);
		void SRA_r8(uint8_t& address);
		void SRA_HL();

		void SRL_r8(uint8_t& address);
		void SRL_HL();

		/* Load Instructions.
		* r8: Any of the 8-bit register(A,B,C,D,E,H,L).
		* r16: Any of the 16-bit register(AF,BC,DE,HL).
		*/
		void LD_r8_r8(uint8_t& address, uint8_t data);
		void LD_r8_n8(uint8_t& address, uint8_t data);
		void LD_r16_n16(uint16_t& address, uint16_t data);
		void LD_HL_r8(uint8_t data);
		void LD_HL_n8(uint8_t data);
		void LD_r8_HL(uint8_t& address);
		void LD_r16_A(uint16_t address);
		void LD_n16_A(uint16_t address);

		void LDH_n16_A(uint16_t address);
		void LDH_C_A();

		void LD_A_r16(uint16_t address);
		void LD_A_n16(uint16_t address);

		void LDH_A_n16(uint16_t address);
		void LDH_A_C();

		void LD_HLI_A();
		void LD_HLD_A();

		void LD_A_HLI();
		void LD_A_HLD();

		/* Jump and Subroutines Instructions.
		* r8: Any of the 8-bit register(A,B,C,D,E,H,L).
		* r16: Any of the 16-bit register(AF,BC,DE,HL).
		* e8: 8-bit offset(-128 ~ 127)
		* cc: Condition code[Z/NZ/C/NC/!CC].
		*/
		void CALL(uint16_t address);
		void CALL_n16(uint16_t address);
		void CALLNZ_n16(uint16_t address);
		void CALLZ_n16(uint16_t address);
		void CALLNC_n16(uint16_t address);
		void CALLC_n16(uint16_t address);

		void JP(uint16_t address);
		void JP_HL();
		void JP_n16(uint16_t address);
		void JPNZ_n16(uint16_t address);
		void JPZ_n16(uint16_t address);
		void JPNC_n16(uint16_t address);
		void JPC_n16(uint16_t address);
		void JP_CC_n16(uint8_t condition_code, uint16_t address);

		void JR(uint8_t data);
		void JR_n8(uint8_t data);
		void JRNZ_n8(uint8_t data);
		void JRZ_n8(uint8_t data);
		void JRNC_n8(uint8_t data);
		void JRC_n8(uint8_t data);
		void JR_e8(uint8_t offset);
		void JR_CC_e8(uint8_t condition_code, uint16_t address);

		void RET();
		void RETI();
		void RETNZ();
		void RETZ();
		void RETNC();
		void RETC();
		void RET_CC(uint8_t condition_code);
		void RET_Impl();
		void RST_VEC(uint16_t address);

		/* Stack operations Instructions.
		*/
		void ADD_HL_SP();
		void ADD_SP_e8(uint8_t offset);

		void DEC_SP();
		void INC_SP();

		void LD_SP_n16(uint16_t data);
		void LD_n16_SP(uint16_t address);
		void LD_HL_SP_e8(uint8_t offset);
		void LD_SP_HL();

		void POP_AF();
		void POP_r16(uint16_t& data);

		void PUSH_AF();
		void PUSH_r16(uint16_t data);

		/* Other Instructions.
		*/
		void CCF();
		void CPL();
		void DAA();
		void DI();
		void EI();
		void HALT();
		void NOP();
		void SCF();
		void STOP();
};
#endif