#include <iostream>
#include <sstream>

#include "Bus.h"
#include "NES6502.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class NES6502_Test : public olc::PixelGameEngine
{
public:
	NES6502_Test() { sAppName = "NES6502 Test"; }

	Bus NES;
	std::map<uint16_t, std::string> mapAsm;

	std::string hex(uint32_t n, uint8_t d)	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};

	void DrawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns)	{
		int nRamX = x, nRamY = y;
		for (int row = 0; row < nRows; row++) {
			std::string sOffset = "$" + hex(nAddr, 4) + ":";
			for (int col = 0; col < nColumns; col++)
			{
				sOffset += " " + hex(NES.read(nAddr, true), 2);
				nAddr += 1;
			}
			DrawString(nRamX, nRamY, sOffset, olc::CYAN);
			nRamY += 10;
		}
	}

	void DrawCpu(int x, int y)	{
		std::string status = "STATUS: ";
		DrawString(x, y, "STATUS:", olc::CYAN);
		DrawString(x + 64, y, "N", NES.CPU.status & NES6502::N ? olc::GREEN : olc::RED);
		DrawString(x + 80, y, "V", NES.CPU.status & NES6502::V ? olc::GREEN : olc::RED);
		DrawString(x + 96, y, "-", NES.CPU.status & NES6502::U ? olc::GREEN : olc::RED);
		DrawString(x + 112, y, "B", NES.CPU.status & NES6502::B ? olc::GREEN : olc::RED);
		DrawString(x + 128, y, "D", NES.CPU.status & NES6502::D ? olc::GREEN : olc::RED);
		DrawString(x + 144, y, "I", NES.CPU.status & NES6502::I ? olc::GREEN : olc::RED);
		DrawString(x + 160, y, "Z", NES.CPU.status & NES6502::Z ? olc::GREEN : olc::RED);
		DrawString(x + 178, y, "C", NES.CPU.status & NES6502::C ? olc::GREEN : olc::RED);
		DrawString(x, y + 10, "PC: $" + hex(NES.CPU.prog_ctr, 4));
		DrawString(x, y + 20, "A: $" + hex(NES.CPU.a, 2) + "  [" + std::to_string(NES.CPU.a) + "]");
		DrawString(x, y + 30, "X: $" + hex(NES.CPU.x, 2) + "  [" + std::to_string(NES.CPU.x) + "]");
		DrawString(x, y + 40, "Y: $" + hex(NES.CPU.y, 2) + "  [" + std::to_string(NES.CPU.y) + "]");
		DrawString(x, y + 50, "Stack P: $" + hex(NES.CPU.stack_ptr, 4));
	}

	void DrawCode(int x, int y, int nLines)	{
		auto it_a = mapAsm.find(NES.CPU.prog_ctr);
		int nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end()) {
			DrawString(x, nLineY, (*it_a).second, olc::DARK_CYAN);
			while (nLineY < (nLines * 10) + y) {
				nLineY += 10;
				if (++it_a != mapAsm.end())	{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}

		it_a = mapAsm.find(NES.CPU.prog_ctr);
		nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end()) {
			while (nLineY > y) {
				nLineY -= 10;
				if (--it_a != mapAsm.end())	{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}
	}

	bool OnUserCreate() {
		// 6502 program to add two numbers and store the result in a specified memory location
		// assembled at: https://www.masswerk.at/6502/assembler.html
		/*
			*=$8000
			CLC
			LDA #04
			ADC #03
			STA $0010
			NOP
		*/

		// Convert hex string into bytes for RAM
		std::stringstream ss;
		ss << "18 A9 04 69 03 8D 10 00 EA";
		uint16_t nOffset = 0x8000;
		while (!ss.eof()) {
			std::string b;
			ss >> b;
			NES.RAM[nOffset++] = (uint8_t)std::stoul(b, nullptr, 16);
		}

		// Set Reset Vector
		NES.RAM[0xFFFC] = 0x00;
		NES.RAM[0xFFFD] = 0x80;

		// Extract dissassembly
		mapAsm = NES.CPU.disassemble(0x0000, 0xFFFF);

		// Reset
		NES.CPU.reset();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) {
		Clear(olc::BLACK);

		if (GetKey(olc::Key::SPACE).bPressed) {
			do { NES.CPU.clock(); } while (!NES.CPU.cyclesLeft == 0);
		}

		if (GetKey(olc::Key::R).bPressed) {	NES.CPU.reset(); }

		if (GetKey(olc::Key::I).bPressed) { NES.CPU.interruptRequest(); }
			
		if (GetKey(olc::Key::N).bPressed) { NES.CPU.nonMaskableInterrupt(); }
		
		DrawRam(2, 2, 0x0000, 16, 16);
		DrawRam(2, 182, 0x8000, 16, 16);
		DrawCpu(448, 2);
		DrawCode(448, 72, 26);
		DrawString(10, 370, "SPACE = Step Instruction    R = RESET    I = IRQ    N = NMI");

		return true;
	}
};

int main(){
	NES6502_Test demo;
	demo.Construct(680, 480, 2, 2);
	demo.Start();
	return 0;
}