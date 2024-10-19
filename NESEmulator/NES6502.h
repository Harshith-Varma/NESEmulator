#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>

// Forward declaration
class Bus;

/**
 * @brief This class represents the CPU of the NES, which is a modified version of the 
 * 6502 processor by MOSChip. This is not a hundred percent accurate representation of the CPU
 * and all of OpCodes, Flags and Addressing Modes, but it is accurate to the extent that allows it
 * to be good enough for NES emulation.
 */
class NES6502
{
public:
	NES6502();
	~NES6502();

	// Helper method that connects CPU instance to Bus instance (glorified setter)
	void ConnectBusToCPU(Bus* bus) {
		this->bus = bus;	
	}

	// One invocation = one clock cycle
	void clock();
	// Interrupts:
	void reset();
	void interruptRequest();
	void nonMaskableInterrupt();

	uint8_t fetch();
	uint8_t fetchedData = 0x00;
	uint16_t absAddress = 0x0000;
	uint16_t relAddress = 0x0000;
	uint8_t currentOpcode = 0x00;
	uint8_t cyclesLeft = 0x00;

	// CPU Flags. Decimal mode not implemented.
	enum FLAGS {
		C = (1 << 0), // Carry Flag
		Z = (1 << 1), // Zero Flag
		I = (1 << 2), // Interrupt Disable
		D = (1 << 3), // Decimal Mode (6502 used in NES does not use Decimal Mode, so I won't implement it)
		B = (1 << 4), // Break
		U = (1 << 5), // Unused
		V = (1 << 6), // Overflow 
		N = (1 << 7), // Negative
	};

	// General and special registers
	uint8_t a = 0x00;			// Accumulator Register
	uint8_t x = 0x00;			// X - Register
	uint8_t y = 0x00;			// Y - Register
	uint8_t stack_ptr = 0x00;	// Stack pointer
	uint8_t status = 0x00;		// Status Register
	uint16_t prog_ctr = 0x00;	// Program Counter

	// Addressing Modes
	uint8_t IMP();  // Implicit
	uint8_t IMM();	// Immediate
	uint8_t ZP0();	// Zero Page
	uint8_t ZPX();	// Zero Page, X
	uint8_t ZPY();  // Zero Page, Y
	uint8_t REL();	// Relative
	uint8_t ABS();	// Absolute 
	uint8_t ABX();	// Absolute, X
	uint8_t ABY();	// Absolute, Y
	uint8_t IND();	// Indirect
	uint8_t IZX();	// Indirect, X
	uint8_t IZY();  // Indirect, Y

	// OpCodes (this one's gonna be long asf). Wrote in order of remembrance, might change to
	// alphabetical if I feel upto it (im a lazy bastard so probably not gonna happen).
	// referred from here: https://www.masswerk.at/6502/6502_instruction_set.html

	uint8_t ADC(); // add with carry
	uint8_t BCS(); // branch when carry bit set
	uint8_t BNE(); // branch if not equal
	uint8_t BVS(); // branch if overflow bit set
	uint8_t CLV(); // clear overflow bit
	uint8_t DEC(); // Decrement (--)
	uint8_t INC(); // Increment (++)
	uint8_t JSR(); // Jump to subroutine (fancy word for function)
	uint8_t LSR(); // logical shift right (>> operator, dgaf about sign bit)
	uint8_t PHP(); // Push Processor Status (PHP lmao)
	uint8_t ROR(); // rotate right
	uint8_t SEC(); // set carry bit
	uint8_t STX(); // Store X in memory
	uint8_t TSX(); // Transfer stack ptr to X
	uint8_t AND(); // And (its literally just and)
	uint8_t BEQ(); // Branch if equal
	uint8_t BPL(); // Branch on plus
	uint8_t CLC(); // Clear carry bit
	uint8_t CMP(); // Compare 
	uint8_t DEX(); // Decrement X register
	uint8_t INX(); // Increment X register
	uint8_t LDA(); // Load accumulator (certified hood classic)
	uint8_t NOP(); // No operation (no official operation, but game devs are goblins so they use "unofficial opCodes")
	uint8_t PLA(); // Pull accumulator
	uint8_t RTI(); // Return from interrupt
	uint8_t SED(); // Set decimal
	uint8_t STY(); // Store Y
	uint8_t TXA(); // Transfer X to accumulator
	uint8_t ASL(); // Arithmetic shift left (<< operator with sign bit preserved)
	uint8_t BIT(); // Bit Test
	uint8_t BRK(); // Force break
	uint8_t CLD(); // Clear decimal
	uint8_t CPX(); // Compare with X register
	uint8_t DEY(); // Decrement Y register
	uint8_t INY(); // Incrrement Y register
	uint8_t LDX(); // Load X
	uint8_t ORA(); // OR with accumulator
	uint8_t PLP(); // Pull processor status
	uint8_t RTS(); // Return from sub-routine
	uint8_t SEI(); // Set interrupt disable
	uint8_t TAX(); // Transfer accumulator to X register (Nirmala's fav)
	uint8_t TXS(); // Transfer X to stack pointer
	uint8_t BCC(); // Branch when carry bit clear
	uint8_t BMI(); // Branch on minus
	uint8_t BVC(); // Branch if overflow bit clear
	uint8_t CLI(); // Clear interrupt disable 
	uint8_t CPY(); // Compare with Y register
	uint8_t EOR(); // XOR (Instead of being normal and calling it XOR, they had to be fancy ffs)
	uint8_t JMP(); // Jump
	uint8_t LDY(); // Load Y register
	uint8_t PHA(); // Push accumulator
	uint8_t ROL(); // Rotate left
	uint8_t SBC(); // Subtract with carry
	uint8_t STA(); // Store accumulator in memory
	uint8_t TAY(); // Transfer accumulator to Y
	uint8_t TYA(); // Transfer Y to accumulator

	uint8_t XXX(); // Invalid
	
	std::map<uint16_t, std::string> disassemble(uint16_t nStart, uint16_t nStop);

private:
	Bus* bus = nullptr;

	// CPU read and write functions; invoke Bus read and write functions.
	// Bit roundabout, but it isolates CPU from Bus so that they can be used standalone later if needed.
	void write(uint16_t address, uint8_t data);
	uint8_t read(uint16_t address, bool readOnly = false);

	// Utility methods to set and get the flag values in the FLAGS enum (make it static maybe?)
	uint8_t getFlag(FLAGS f);
	void setFlag(FLAGS f, bool value);

	/**
	 * @brief Struct that represents an instruction.  
	 */
	struct Instruction {
		std::string insName;								// Instruction Name
		uint8_t(NES6502::* opCode)(void) = nullptr;		// Function pointer for OpCode
		uint8_t(NES6502::* addressingMode)(void) = nullptr; // Function pointer for addressingMode
		uint8_t instructionCycles = 0;						// Number of cycles required to perform instruction
	};
	std::vector<Instruction> lookup;
};

