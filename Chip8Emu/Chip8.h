#pragma once
#include <string>
#include <fstream>
#include <time.h>

class Chip8 {
	// There are 35 2 byte opcodes
	unsigned short opcode;

	// 4k memory and 15 8 bit registers
	unsigned char memory[4096] = { 0 };
	unsigned char V[16] = { 0 };

	// Index register and program counter
	unsigned short I;
	unsigned short pc;

	// Display is 64x32, each pixel is either on or off
	unsigned char gfx[64 * 32] = { 0 };

	// Both timers are decremented at 60Hz
	unsigned char delay_timer;
	unsigned char sound_timer;

	// Keep track of program jumps
	unsigned short stack[16] = { 0 };
	unsigned short sp;

	// Store the current keystates
	unsigned char key[16] = { 0 };

public:
	// Public flags
	bool drawFlag;

	// Emulator functions
	void initialize();
	void loadFontset();
	void loadGame(std::string path);
	void emulateCycle();
	unsigned char* getGfx();
	void setKeys(unsigned char* newKeys);

	// Opcode specific functions
	void drawSprite();
	void clearScreen();
};