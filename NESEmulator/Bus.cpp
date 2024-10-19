#include "Bus.h"
#include "Definitions.h"

Bus::Bus() {
	for (auto& mem : RAM) { mem = 0; }
	CPU.ConnectBusToCPU(this);
}

Bus::~Bus() {

}

void Bus::write(uint16_t address, uint8_t data) {	
	RAM[address] = checkAddressRange(address) ? data : 0x0000;
}

uint8_t Bus::read(uint16_t address, bool readOnly) {
	return checkAddressRange(address) ? RAM[address] : 0x0000;
}

bool Bus::checkAddressRange(uint16_t address) {
	return (address >= MIN_RAM_VALUE && address <= MAX_RAM_VALUE);
}
