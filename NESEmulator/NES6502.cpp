#include "NES6502.h"
#include "Bus.h"
#include "Definitions.h"

/**
 * @brief WARNING!!!!!!!!!!
 * THIS IS NOT GRACEFUL CODE. THIS IS NOT CLEAN CODE. THIS IS THE ONLY WAY TO DO THIS PERFORMANTLY IN C++.
 * I WILL FIGURE OUT ANOTHER WAY TO DO THIS (eventually), BUT DO NOT LOOK AT THIS CONSTRUCTOR. YOU HAVE BEEN
 * WARNED.
 */
NES6502::NES6502() {
	// Aliasing NES6502 to make this monstrosity of a lookup table slightly smaller.
	using a = NES6502;

	// This lookup table is an initialiser list of initialiser lists. It is esentially a representation of 
	// the opCode table of the NES 6502 CPU, found here: https://www.masswerk.at/6502/6502_instruction_set.html
	lookup =
	{
		{ "BRK", &a::BRK, &a::IMM, 7 },{ "ORA", &a::ORA, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::ZP0, 3 },{ "ASL", &a::ASL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHP", &a::PHP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::IMM, 2 },{ "ASL", &a::ASL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABS, 4 },{ "ASL", &a::ASL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BPL", &a::BPL, &a::REL, 2 },{ "ORA", &a::ORA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ZPX, 4 },{ "ASL", &a::ASL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLC", &a::CLC, &a::IMP, 2 },{ "ORA", &a::ORA, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABX, 4 },{ "ASL", &a::ASL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "JSR", &a::JSR, &a::ABS, 6 },{ "AND", &a::AND, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "BIT", &a::BIT, &a::ZP0, 3 },{ "AND", &a::AND, &a::ZP0, 3 },{ "ROL", &a::ROL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLP", &a::PLP, &a::IMP, 4 },{ "AND", &a::AND, &a::IMM, 2 },{ "ROL", &a::ROL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "BIT", &a::BIT, &a::ABS, 4 },{ "AND", &a::AND, &a::ABS, 4 },{ "ROL", &a::ROL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BMI", &a::BMI, &a::REL, 2 },{ "AND", &a::AND, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ZPX, 4 },{ "ROL", &a::ROL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEC", &a::SEC, &a::IMP, 2 },{ "AND", &a::AND, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ABX, 4 },{ "ROL", &a::ROL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTI", &a::RTI, &a::IMP, 6 },{ "EOR", &a::EOR, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "EOR", &a::EOR, &a::ZP0, 3 },{ "LSR", &a::LSR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHA", &a::PHA, &a::IMP, 3 },{ "EOR", &a::EOR, &a::IMM, 2 },{ "LSR", &a::LSR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::ABS, 3 },{ "EOR", &a::EOR, &a::ABS, 4 },{ "LSR", &a::LSR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVC", &a::BVC, &a::REL, 2 },{ "EOR", &a::EOR, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ZPX, 4 },{ "LSR", &a::LSR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLI", &a::CLI, &a::IMP, 2 },{ "EOR", &a::EOR, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ABX, 4 },{ "LSR", &a::LSR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTS", &a::RTS, &a::IMP, 6 },{ "ADC", &a::ADC, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ADC", &a::ADC, &a::ZP0, 3 },{ "ROR", &a::ROR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLA", &a::PLA, &a::IMP, 4 },{ "ADC", &a::ADC, &a::IMM, 2 },{ "ROR", &a::ROR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::IND, 5 },{ "ADC", &a::ADC, &a::ABS, 4 },{ "ROR", &a::ROR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVS", &a::BVS, &a::REL, 2 },{ "ADC", &a::ADC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ZPX, 4 },{ "ROR", &a::ROR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEI", &a::SEI, &a::IMP, 2 },{ "ADC", &a::ADC, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ABX, 4 },{ "ROR", &a::ROR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "???", &a::NOP, &a::IMP, 2 },{ "STA", &a::STA, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZP0, 3 },{ "STA", &a::STA, &a::ZP0, 3 },{ "STX", &a::STX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "DEY", &a::DEY, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 2 },{ "TXA", &a::TXA, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "STY", &a::STY, &a::ABS, 4 },{ "STA", &a::STA, &a::ABS, 4 },{ "STX", &a::STX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCC", &a::BCC, &a::REL, 2 },{ "STA", &a::STA, &a::IZY, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZPX, 4 },{ "STA", &a::STA, &a::ZPX, 4 },{ "STX", &a::STX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "TYA", &a::TYA, &a::IMP, 2 },{ "STA", &a::STA, &a::ABY, 5 },{ "TXS", &a::TXS, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::NOP, &a::IMP, 5 },{ "STA", &a::STA, &a::ABX, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::XXX, &a::IMP, 5 },
		{ "LDY", &a::LDY, &a::IMM, 2 },{ "LDA", &a::LDA, &a::IZX, 6 },{ "LDX", &a::LDX, &a::IMM, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "LDY", &a::LDY, &a::ZP0, 3 },{ "LDA", &a::LDA, &a::ZP0, 3 },{ "LDX", &a::LDX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "TAY", &a::TAY, &a::IMP, 2 },{ "LDA", &a::LDA, &a::IMM, 2 },{ "TAX", &a::TAX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "LDY", &a::LDY, &a::ABS, 4 },{ "LDA", &a::LDA, &a::ABS, 4 },{ "LDX", &a::LDX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCS", &a::BCS, &a::REL, 2 },{ "LDA", &a::LDA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "LDY", &a::LDY, &a::ZPX, 4 },{ "LDA", &a::LDA, &a::ZPX, 4 },{ "LDX", &a::LDX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "CLV", &a::CLV, &a::IMP, 2 },{ "LDA", &a::LDA, &a::ABY, 4 },{ "TSX", &a::TSX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 4 },{ "LDY", &a::LDY, &a::ABX, 4 },{ "LDA", &a::LDA, &a::ABX, 4 },{ "LDX", &a::LDX, &a::ABY, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "CPY", &a::CPY, &a::IMM, 2 },{ "CMP", &a::CMP, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPY", &a::CPY, &a::ZP0, 3 },{ "CMP", &a::CMP, &a::ZP0, 3 },{ "DEC", &a::DEC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INY", &a::INY, &a::IMP, 2 },{ "CMP", &a::CMP, &a::IMM, 2 },{ "DEX", &a::DEX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "CPY", &a::CPY, &a::ABS, 4 },{ "CMP", &a::CMP, &a::ABS, 4 },{ "DEC", &a::DEC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BNE", &a::BNE, &a::REL, 2 },{ "CMP", &a::CMP, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ZPX, 4 },{ "DEC", &a::DEC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLD", &a::CLD, &a::IMP, 2 },{ "CMP", &a::CMP, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ABX, 4 },{ "DEC", &a::DEC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "CPX", &a::CPX, &a::IMM, 2 },{ "SBC", &a::SBC, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPX", &a::CPX, &a::ZP0, 3 },{ "SBC", &a::SBC, &a::ZP0, 3 },{ "INC", &a::INC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INX", &a::INX, &a::IMP, 2 },{ "SBC", &a::SBC, &a::IMM, 2 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::SBC, &a::IMP, 2 },{ "CPX", &a::CPX, &a::ABS, 4 },{ "SBC", &a::SBC, &a::ABS, 4 },{ "INC", &a::INC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BEQ", &a::BEQ, &a::REL, 2 },{ "SBC", &a::SBC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ZPX, 4 },{ "INC", &a::INC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SED", &a::SED, &a::IMP, 2 },{ "SBC", &a::SBC, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ABX, 4 },{ "INC", &a::INC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
	};
}
NES6502::~NES6502() {}

void NES6502::write(uint16_t address, uint8_t data) {
	bus->write(address, data);
}

uint8_t NES6502::read(uint16_t address, bool readOnly) {
	return bus->read(address, readOnly);
}

void NES6502::clock() {
	if (cyclesLeft == 0) {
		currentOpcode = read(prog_ctr);
		prog_ctr++;

		// Set cyclesLeft to number of cycles required to complete this instruction.
		cyclesLeft = lookup[currentOpcode].instructionCycles;
		// Use function pointers to async call the operation and its associated addressing mode.
		// If the additionalCycle return values are non-zero, we add them to the number of cycles left. 
		uint8_t additionalCycle1 = (this->*lookup[currentOpcode].addressingMode)();
		uint8_t additionalCycle2 = (this->*lookup[currentOpcode].opCode)();
		cyclesLeft += additionalCycle1 + additionalCycle2;
	}
	cyclesLeft--;
}

// Gets a specific bit of the status register
uint8_t NES6502::getFlag(FLAGS f) {
	return ((status & f) > 0) ? 1 : 0;
}

// Sets or clears a specific bit of the status register
void NES6502::setFlag(FLAGS f, bool v) {
	if (v) { status |= f; }
	else { status &= ~f; }
}

// Addressing Modes:

uint8_t NES6502::IMP() {
	fetchedData = a;

	return 0x00;
}

uint8_t NES6502::IMM() {
	absAddress = prog_ctr++;

	return 0x00;
}

uint8_t NES6502::ZP0() {
	absAddress = read(prog_ctr++);
	absAddress &= ZERO_PAGE_MAX_OFFSET;

	return 0x00;
}

uint8_t NES6502::ZPX() {
	absAddress = (read(prog_ctr++) + x);
	prog_ctr++;
	absAddress &= ZERO_PAGE_MAX_OFFSET;

	return 0x00;
}

uint8_t NES6502::ZPY() {
	absAddress = (read(prog_ctr++) + y);
	prog_ctr++;
	absAddress &= ZERO_PAGE_MAX_OFFSET;

	return 0x00;
}

uint8_t NES6502::REL() {
	relAddress = read(prog_ctr++);

	//Relative addressing does not follow the same sign representation; set the high bit accordingly.
	if (relAddress & 0x80) { relAddress |= 0x80; }

	return 0x00;
}

uint8_t NES6502::ABS() { 
	uint16_t lowBits = read(prog_ctr++);
	uint16_t highBits = read(prog_ctr++);

	// left shift highBits and fill the vacant spaces with lowBits
	absAddress = (highBits << 8) | lowBits;

	return 0x00;
}

uint8_t NES6502::ABX() {
	uint16_t lowBits = read(prog_ctr++);
	uint16_t highBits = read(prog_ctr++);

	// left shift highBits and fill the vacant spaces with lowBits, then offset by X
	absAddress = (highBits << 8) | lowBits;
	absAddress += x;

	// check if the page has changed after offsetting the address by X, and indicate accordingly 
	// by returning 1; this will add an additional clock cycle to this instruction
	return ((absAddress & PAGE_CHANGE_BY_OVERFLOW) != (highBits << 8)) ? 1 : 0x00;
}

uint8_t NES6502::ABY() {
	uint16_t lowBits = read(prog_ctr++);
	uint16_t highBits = read(prog_ctr++);

	// left shift highBits and fill the vacant spaces with lowBits, then offset by X
	absAddress = (highBits << 8) | lowBits;
	absAddress += y;

	// check if the page has changed after offsetting the address by X, and indicate accordingly 
	// by returning 1; this will add an additional clock cycle to this instruction
	return ((absAddress & PAGE_CHANGE_BY_OVERFLOW) != (highBits << 8)) ? 1 : 0x00;
}

uint8_t NES6502::IND() {
	// This is basically pointer addressing.
	uint16_t lowPointerBits = read(prog_ctr++);
	uint16_t highPointerBits = read(prog_ctr++);
	uint16_t pointer = (highPointerBits << 8) | lowPointerBits;

	// This if condition is needed to emulate a bug that is present in the original NES 6502, where
	// incrementing the higher 2 bits of the high address would shift the page of the memory address.
	// Futher details can be found here: https://www.nesdev.org/6502bugs.txt.
	// 
	// Some games rely on this bug, so we need to introduce this bug. (peak game dev brainrot)
	if (lowPointerBits == ZERO_PAGE_MAX_OFFSET) { 
		absAddress = (read(pointer & PAGE_CHANGE_BY_OVERFLOW) << 8) | read(pointer + 0); 
	} 
	else { absAddress = read((pointer + 1) << 8) | read(pointer + 0); }

	return 0x00;
}

uint8_t NES6502::IZX() {
	// Pointer addressing with X offset.
	uint16_t temp = read(prog_ctr++);
	uint16_t lowPointerBits = read((uint16_t)(temp + (uint16_t)x) & ZERO_PAGE_MAX_OFFSET);
	uint16_t highPointerBits = read((uint16_t)(temp + (uint16_t)x + 1) & ZERO_PAGE_MAX_OFFSET);

	absAddress = (highPointerBits << 8) | lowPointerBits;

	return 0x00;
}

uint8_t NES6502::IZY() {
	//Pointer addressing with Y offset.
	uint16_t temp = read(prog_ctr++);
	uint16_t lowPointerBits = read(temp & ZERO_PAGE_MAX_OFFSET);
	uint16_t highPointerBits = read((temp + 1) & ZERO_PAGE_MAX_OFFSET);

	absAddress = (highPointerBits << 8) | lowPointerBits;
	absAddress += y;

	// check if the page has changed after offsetting the address by X, and indicate accordingly 
	// by returning 1; this will add an additional clock cycle to this instruction
	return ((absAddress & PAGE_CHANGE_BY_OVERFLOW) != (highPointerBits << 8)) ? 1 : 0x00;
}

// Instructions:

uint8_t NES6502::fetch() {
	// fetch data for all adddressing modes except Implicit.
	if (lookup[currentOpcode].addressingMode != &NES6502::IMP) { fetchedData = read(absAddress); }

	// Return is for convenience; allows chaining.
	return fetchedData;
}

uint8_t NES6502::AND() {
	fetch();
	a = a & fetchedData;

	setFlag(Z, a == 0x00);
	setFlag(N, a & 0x80);
	
	// Can potentially add more clock cycles if page overflow occurs
	return 1;
}

uint8_t NES6502::BCS() {
	if (getFlag(C) == 1) {
		// if branching needed, increase clock cycles needed by 1
		cyclesLeft++;
		absAddress = prog_ctr + relAddress;

		// if branching crosses page boundary, increase clock by another 1
		// this logic is applicable to all branching instructions.
		if ((absAddress & PAGE_CHANGE_BY_OVERFLOW) != (prog_ctr & PAGE_CHANGE_BY_OVERFLOW)) { cyclesLeft++; }

		prog_ctr = absAddress;
	}
	return 0x00;
}

uint8_t NES6502::BEQ() {
	if (getFlag(Z) == 1) {
		cyclesLeft++;
		absAddress = prog_ctr + relAddress;

		if ((absAddress & PAGE_CHANGE_BY_OVERFLOW) != (prog_ctr & PAGE_CHANGE_BY_OVERFLOW)) { cyclesLeft++; }

		prog_ctr = absAddress;
	}
	return 0x00;
}

uint8_t NES6502::BMI() {
	if (getFlag(N) == 1) {
		cyclesLeft++;
		absAddress = prog_ctr + relAddress;

		if ((absAddress & PAGE_CHANGE_BY_OVERFLOW) != (prog_ctr & PAGE_CHANGE_BY_OVERFLOW)) { cyclesLeft++; }

		prog_ctr = absAddress;
	}
	return 0x00;
}

uint8_t NES6502::BNE() {
	if (getFlag(Z) == 0) {
		cyclesLeft++;
		absAddress = prog_ctr + relAddress;

		if ((absAddress & PAGE_CHANGE_BY_OVERFLOW) != (prog_ctr & PAGE_CHANGE_BY_OVERFLOW)) { cyclesLeft++; }

		prog_ctr = absAddress;
	}
	return 0x00;
}

uint8_t NES6502::BPL() {
	if (getFlag(N) == 0) {
		cyclesLeft++;
		absAddress = prog_ctr + relAddress;

		if ((absAddress & PAGE_CHANGE_BY_OVERFLOW) != (prog_ctr & PAGE_CHANGE_BY_OVERFLOW)) { cyclesLeft++; }

		prog_ctr = absAddress;
	}
	return 0x00;
}

uint8_t NES6502::BVC() {
	if (getFlag(V) == 0) {
		cyclesLeft++;
		absAddress = prog_ctr + relAddress;

		if ((absAddress & PAGE_CHANGE_BY_OVERFLOW) != (prog_ctr & PAGE_CHANGE_BY_OVERFLOW)) { cyclesLeft++; }

		prog_ctr = absAddress;
	}
	return 0x00;
}

uint8_t NES6502::BVS() {
	if (getFlag(V) == 1	) {
		cyclesLeft++;
		absAddress = prog_ctr + relAddress;

		if ((absAddress & PAGE_CHANGE_BY_OVERFLOW) != (prog_ctr & PAGE_CHANGE_BY_OVERFLOW)) { cyclesLeft++; }

		prog_ctr = absAddress;
	}
	return 0x00;
}

uint8_t NES6502::CLC() {
	setFlag(C, false);
	return 0x00;
}

uint8_t NES6502::CLD() {
	setFlag(D, false);
	return 0x00;
}

uint8_t NES6502::CLV() {
	setFlag(V, false);
	return 0x00;
}

uint8_t NES6502::CLI() {
	setFlag(I, false);
	return 0x00;
}

uint8_t NES6502::ADC() {
	fetch();
	// Add value in accumulator to fetched data in memory and carry flag to carry if needed
	uint16_t temp = (uint16_t)a + (uint16_t)fetchedData + (uint16_t)getFlag(C);
	// if result overflows, set carry flag
	(temp > 255) ? setFlag(C, true) : setFlag(C, false);
	// if page zero, set zero flag
	((temp & ZERO_PAGE_MAX_OFFSET) == 0) ? setFlag(Z, true) : setFlag(Z, false);
	// set negative flag based on MSB of low byte (8th bit from 0)
	setFlag(N, temp & 0x80);

	// This monstrosity was derived after drawing a bunch of logic tables and arriving at the
	// condition to set the overflow flag. 
	// Will update this comment with more info later, feeling too lazy to do it now.
	setFlag(V, (~((uint16_t)a ^ (uint16_t)fetchedData) & ((uint16_t)a ^ temp)) & 0x0080);

	a = temp & ZERO_PAGE_MAX_OFFSET;

	// ADC can increment cycles, so we return 1
	return 1;
}

uint8_t NES6502::SBC() {
	fetch();

	// Subtract is the same as add, but with an inverted value so that we can add the 2s complement instead. 
	uint16_t invFetchedData = ((uint16_t)fetchedData) ^ ZERO_PAGE_MAX_OFFSET;

	uint16_t temp = (uint16_t)a + (uint16_t)invFetchedData + (uint16_t)getFlag(C);
	(temp > 255) ? setFlag(C, true) : setFlag(C, false);
	((temp & ZERO_PAGE_MAX_OFFSET) == 0) ? setFlag(Z, true) : setFlag(Z, false);
	setFlag(N, temp & 0x80);

	setFlag(V, (~((uint16_t)a ^ invFetchedData) & ((uint16_t)a ^ temp)) & 0x0080);
	
	a = temp & ZERO_PAGE_MAX_OFFSET;

	// SBC can increment cycles, so we return 1
	return 1;
}

uint8_t NES6502::PHA() {
	// 0x0100 is the base memory location of the stack; stack pointer is
	// used to offset.
	write(STACK_BASE_MEMORY_LOCATION + stack_ptr, a);
	stack_ptr--;

	return 0x00;
}

uint8_t NES6502::PLA() {
	stack_ptr++;
	a = read(STACK_BASE_MEMORY_LOCATION + stack_ptr);
	(a == 0x00) ? setFlag(Z, true) : setFlag(Z, false);
	setFlag(N, a & 0x80);

	return 0x00; 
}

uint8_t NES6502::STX() {
	write(absAddress, x);
	return 0x00;
}

uint8_t NES6502::STA() {
	write(absAddress, a);
	return 0x00;
}

uint8_t NES6502::LDX() {
	fetch();
	x = fetchedData;
	(x == 0x00) ? setFlag(Z, true) : setFlag(Z, false);
	setFlag(N, x & 0x80);

	// Could increment clock cycles, so we return 1
	return 1;
}

uint8_t NES6502::LDY() {
	fetch();
	y = fetchedData;
	(y == 0x00) ? setFlag(Z, true) : setFlag(Z, false);
	setFlag(N, y & 0x80);

	// Could increment clock cycles, so we return 1
	return 1;
}

uint8_t NES6502::LDA() {
	fetch();
	a = fetchedData;
	(a == 0x00) ? setFlag(Z, true) : setFlag(Z, false);	
	setFlag(N, a & 0x80);

	// Could increment clock cycles, so we return 1
	return 1;
}

uint8_t NES6502::DEY()
{
	y--;
	(y == 0x00) ? setFlag(Z, true) : setFlag(Z, false);
	setFlag(N, y & 0x80);

	return 0x00;
}

uint8_t NES6502::INC() {
	fetch();
	uint8_t temp = fetchedData + 1;
	write(absAddress, temp & ZERO_PAGE_MAX_OFFSET);
	((temp & ZERO_PAGE_MAX_OFFSET) == 0x00) ? setFlag(Z, true) : setFlag(Z, false);
	setFlag(N, temp & 0x0080);

	return 0x00;
}

uint8_t NES6502::DEC() {
	fetch();
	uint8_t temp = fetchedData - 1;
	write(absAddress, temp & ZERO_PAGE_MAX_OFFSET);
	((temp & ZERO_PAGE_MAX_OFFSET) == 0x00) ? setFlag(Z, true) : setFlag(Z, false);
	setFlag(N, temp & 0x0080);

	return 0x00;
}

uint8_t NES6502::JSR() {
	prog_ctr--;

	write(STACK_BASE_MEMORY_LOCATION + stack_ptr, (prog_ctr >> 8) & ZERO_PAGE_MAX_OFFSET);
	stack_ptr--;
	write(STACK_BASE_MEMORY_LOCATION + stack_ptr, prog_ctr & ZERO_PAGE_MAX_OFFSET);
	stack_ptr--;

	prog_ctr = absAddress;
	return 0;
}

uint8_t NES6502::LSR()
{
	fetch();
	setFlag(C, fetchedData & 0x0001);
	uint8_t temp = fetchedData >> 1;

	((temp & ZERO_PAGE_MAX_OFFSET) == 0x00) ? setFlag(Z, true) : setFlag(Z, false);
	setFlag(N, temp & 0x0080);

	if (lookup[currentOpcode].addressingMode == &NES6502::IMP) { a = temp & ZERO_PAGE_MAX_OFFSET; }
	else { write(absAddress, temp & ZERO_PAGE_MAX_OFFSET); }

	return 0x00;
}

uint8_t NES6502::PHP() {
	write(STACK_BASE_MEMORY_LOCATION + stack_ptr, status | B | U);
	setFlag(B, 0);
	setFlag(U, 0);
	stack_ptr--;

	return 0x00;
}

uint8_t NES6502::ROR() { return 0x00; }
uint8_t NES6502::SEC() { return 0x00; }
uint8_t NES6502::TSX() { return 0x00; }
uint8_t NES6502::CMP() { return 0x00; }
uint8_t NES6502::DEX() { return 0x00; }
uint8_t NES6502::INX() { return 0x00; }
uint8_t NES6502::SED() { return 0x00; }
uint8_t NES6502::STY() { return 0x00; }
uint8_t NES6502::TXA() { return 0x00; }
uint8_t NES6502::ASL() { return 0x00; }
uint8_t NES6502::BIT() { return 0x00; }
uint8_t NES6502::BRK() { return 0x00; }
uint8_t NES6502::CPX() { return 0x00; }
uint8_t NES6502::INY() { return 0x00; }
uint8_t NES6502::ORA() { return 0x00; }
uint8_t NES6502::PLP() { return 0x00; }
uint8_t NES6502::RTS() { return 0x00; }
uint8_t NES6502::SEI() { return 0x00; }
uint8_t NES6502::TAX() { return 0x00; }
uint8_t NES6502::TXS() { return 0x00; }
uint8_t NES6502::BCC() { return 0x00; }
uint8_t NES6502::CPY() { return 0x00; }
uint8_t NES6502::EOR() { return 0x00; }
uint8_t NES6502::JMP() { return 0x00; }
uint8_t NES6502::ROL() { return 0x00; }
uint8_t NES6502::TAY() { return 0x00; }
uint8_t NES6502::TYA() { return 0x00; }

uint8_t NES6502::XXX() { return 0; }

uint8_t NES6502::NOP() {
	switch (currentOpcode) {
		case 0x1C:
		case 0x3C:
		case 0x5C:
		case 0x7C:
		case 0xDC:
		case 0xFC:
			return 1;
			break;
	}
	return 0x00;
}

void NES6502::reset() {
	// Kind of a destructor
	a = 0x00;
	x = 0x00;
	y = 0x00;
	stack_ptr = 0xFD;
	status = 0x00 | U;

	// Default address for program counter to point to in NES6502; hardcoded
	absAddress = PROGRAM_COUNTER_DEFAULT_LOCATION_RESET;
	uint16_t lowBits = read(absAddress);
	uint16_t highBits = read(absAddress + 1);
	prog_ctr = (highBits << 8) | lowBits;

	absAddress = 0x00;
	relAddress = 0x00;
	fetchedData = 0x00;

	// Resets require 8 cycles.
	cyclesLeft = 8;
}

void NES6502::interruptRequest() {
	if (getFlag(I) == 0) {		
		// Write cuurent program counter to stack; requires two writes because only 1 byte
		// can be written per write
		write(STACK_BASE_MEMORY_LOCATION + stack_ptr, (prog_ctr >> 8) & ZERO_PAGE_MAX_OFFSET);
		stack_ptr--;
		write(STACK_BASE_MEMORY_LOCATION + stack_ptr, prog_ctr & ZERO_PAGE_MAX_OFFSET);
		stack_ptr++;

		// Set flags and write the status register to the stack
		setFlag(B, 0);
		setFlag(U, 1);
		setFlag(I, 1);
		write(STACK_BASE_MEMORY_LOCATION + stack_ptr, status);
		stack_ptr--;

		// Set the program counter location to fixed address 0xFFEE
		absAddress = INTERRUPT_REQUEST_DEFAULT_ADDRESS;
		uint16_t lowBits = read(absAddress);
		uint16_t highBits = read(absAddress + 1);
		prog_ctr = (highBits << 8) | lowBits;

		// Interrupt requires 7 cycles.
		cyclesLeft = 7;
	}
}

void NES6502::nonMaskableInterrupt() {
	// Same as interruptRequest, but cannot be stopped by I flag being set.
	write(STACK_BASE_MEMORY_LOCATION + stack_ptr, (prog_ctr >> 8) & ZERO_PAGE_MAX_OFFSET);
	stack_ptr--;
	write(STACK_BASE_MEMORY_LOCATION + stack_ptr, prog_ctr & ZERO_PAGE_MAX_OFFSET);
	stack_ptr++;

	setFlag(B, 0);
	setFlag(U, 1);
	setFlag(I, 1);
	write(STACK_BASE_MEMORY_LOCATION + stack_ptr, status);
	stack_ptr--;

	absAddress = 0xFFEE;
	uint16_t lowBits = read(absAddress);
	uint16_t highBits = read(absAddress + 1);
	prog_ctr = (highBits << 8) | lowBits;

	// Non-maskable Interrupt requires 8 cycles.
	cyclesLeft = 8;
}

uint8_t NES6502::RTI() {
	// To return from interrupt state, we need to read the stuff we stored in the stack when the
	// interrupt was called to restore to state before interrupt.
	stack_ptr++;
	status = read(STACK_BASE_MEMORY_LOCATION + stack_ptr);
	status &= ~B;
	status &= ~U;

	stack_ptr++;
	prog_ctr = (uint16_t)read(STACK_BASE_MEMORY_LOCATION + stack_ptr);
	stack_ptr++;
	prog_ctr |= (uint16_t)read(STACK_BASE_MEMORY_LOCATION + stack_ptr) << 8;

	return 0x00;
}

std::map<uint16_t, std::string> NES6502::disassemble(uint16_t nStart, uint16_t nStop)
{
	uint32_t addr = nStart;
	uint8_t value = 0x00, lo = 0x00, hi = 0x00;
	std::map<uint16_t, std::string> mapLines;
	uint16_t line_addr = 0;

	auto hex = [](uint32_t n, uint8_t d) {
			std::string s(d, '0');
			for (int i = d - 1; i >= 0; i--, n >>= 4)
				s[i] = "0123456789ABCDEF"[n & 0xF];
			return s;
	};

	while (addr <= (uint32_t)nStop) {
		line_addr = addr;

		// Prefix line with instruction address
		std::string sInst = "$" + hex(addr, 4) + ": ";

		// Read instruction, and get its readable name
		uint8_t opcode = bus->read(addr, true); addr++;
		sInst += lookup[opcode].insName + " ";

		// Get oprands from desired locations, and form the
		// instruction based upon its addressing mode. These
		// routines mimmick the actual fetch routine of the
		// 6502 in order to get accurate data as part of the
		// instruction
		if (lookup[opcode].addressingMode == &NES6502::IMP)
		{
			sInst += " {IMP}";
		}
		else if (lookup[opcode].addressingMode == &NES6502::IMM)
		{
			value = bus->read(addr, true); addr++;
			sInst += "#$" + hex(value, 2) + " {IMM}";
		}
		else if (lookup[opcode].addressingMode == &NES6502::ZP0)
		{
			lo = bus->read(addr, true); addr++;
			hi = 0x00;
			sInst += "$" + hex(lo, 2) + " {ZP0}";
		}
		else if (lookup[opcode].addressingMode == &NES6502::ZPX)
		{
			lo = bus->read(addr, true); addr++;
			hi = 0x00;
			sInst += "$" + hex(lo, 2) + ", X {ZPX}";
		}
		else if (lookup[opcode].addressingMode == &NES6502::ZPY)
		{
			lo = bus->read(addr, true); addr++;
			hi = 0x00;
			sInst += "$" + hex(lo, 2) + ", Y {ZPY}";
		}
		else if (lookup[opcode].addressingMode == &NES6502::IZX)
		{
			lo = bus->read(addr, true); addr++;
			hi = 0x00;
			sInst += "($" + hex(lo, 2) + ", X) {IZX}";
		}
		else if (lookup[opcode].addressingMode == &NES6502::IZY)
		{
			lo = bus->read(addr, true); addr++;
			hi = 0x00;
			sInst += "($" + hex(lo, 2) + "), Y {IZY}";
		}
		else if (lookup[opcode].addressingMode == &NES6502::ABS)
		{
			lo = bus->read(addr, true); addr++;
			hi = bus->read(addr, true); addr++;
			sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + " {ABS}";
		}
		else if (lookup[opcode].addressingMode == &NES6502::ABX)
		{
			lo = bus->read(addr, true); addr++;
			hi = bus->read(addr, true); addr++;
			sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", X {ABX}";
		}
		else if (lookup[opcode].addressingMode == &NES6502::ABY)
		{
			lo = bus->read(addr, true); addr++;
			hi = bus->read(addr, true); addr++;
			sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", Y {ABY}";
		}
		else if (lookup[opcode].addressingMode == &NES6502::IND)
		{
			lo = bus->read(addr, true); addr++;
			hi = bus->read(addr, true); addr++;
			sInst += "($" + hex((uint16_t)(hi << 8) | lo, 4) + ") {IND}";
		}
		else if (lookup[opcode].addressingMode == &NES6502::REL)
		{
			value = bus->read(addr, true); addr++;
			sInst += "$" + hex(value, 2) + " [$" + hex(addr + value, 4) + "] {REL}";
		}

		// Add the formed string to a std::map, using the instruction's
		// address as the key.
		mapLines[line_addr] = sInst;
	}

	return mapLines;
}

