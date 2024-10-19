#pragma once

#include <cstdint>
#include <array>
#include "NES6502.h"

class Bus
{
public:
	Bus();
	~Bus();

	// CPU instance
	NES6502 CPU;
	// Fake RAM
	std::array<uint8_t, 64 * 1028> RAM;

	// Bus read and write functions
	void write(uint16_t address, uint8_t data);
	uint8_t read(uint16_t address, bool readOnly = false);

	// Checks whether provided address is present in the 6502's addressable memory range
	bool checkAddressRange(uint16_t address);

};

