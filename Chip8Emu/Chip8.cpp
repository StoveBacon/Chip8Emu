#include "Chip8.h"

void Chip8::initialize() {
	opcode = 0;
	I = 0;
	pc = 0x200;
	delay_timer = 0;
	sound_timer = 0;
	sp = 0;
	drawFlag = false;
	srand(time(NULL));
	loadFontset();
}

void Chip8::loadGame(std::string path) {
	std::ifstream file;
	file.open(path, std::ios::binary);

	if (file.is_open()) {
		// ROM memory starts at location 512 (0x200)
		int offset = 512;
		while (!file.eof()) {
			memory[offset] = file.get();
			offset++;
		}
		file.close();
	}
}

void Chip8::setKeys(unsigned char* newKeys) {
	for (int i = 0; i < sizeof(key); i++) {
		key[i] = *(newKeys + i);
	}
}

void Chip8::emulateCycle() {

	// Fetch opcode (next 2 bytes of memory)
	opcode = memory[pc] << 8 | memory[pc + 1];

	// Check the first digit of the opcode
	switch (opcode & 0xF000) {

	case 0x0000:
		switch (opcode & 0x00FF) {
		case 0x00E0: // (00E0) Clears the screen.
			clearScreen();
			pc += 2;
			break;

		case 0x00EE: // (00EE) Returns from a subroutine.
			sp--;
			pc = stack[sp];
			pc += 2;
			break;
		}
		break;

	case 0x1000: // (1NNN) Jump to address NNN
		pc = (opcode & 0x0FFF);
		break;

	case 0x2000: // (2NNN) Call subroutine at NNN
		stack[sp] = pc;
		sp++;
		pc = (opcode & 0x0FFF);
		break;

	case 0x3000: // (3XNN) Skips the next instruction if VX equals NN
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
			pc += 4;
		} else {
			pc += 2;
		}
		break;

	case 0x4000: // (4XNN) Skips the next instruction if VX doesn't equal NN.
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
			pc += 4;
		} else {
			pc += 2;
		}
		break;

	case 0x5000: // (5XY0) Skips the next instruction if VX equals VY
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
			pc += 4;
		} else {
			pc += 2;
		}
		break;

	case 0x6000: // (6XNN) Sets VX to NN
		V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
		pc += 2;
		break;

	case 0x7000: // (7XNN) Adds NN to VX
		V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
		pc += 2;
		break;

	case 0x8000: // There are multiple cases for a leading 8
		switch (opcode & 0x000F) {

		case 0x0000: // (8XY0) Sets VX to the value of VY
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0001: // (8XY1) Sets VX to VX | VY
			V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4]);
			pc += 2;
			break;

		case 0x0002: // (8XY2) Sets VX to VX & VY
			V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4]);
			pc += 2;
			break;
			
		case 0x0003: // (8XY3) Sets VX to VX ^ VY (XOR)
			V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4]);
			pc += 2;
			break;

		case 0x004: // (8XY4) Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
			if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) {
				V[0xF] = 1; // Carry
			} else {
				V[0xF] = 0;
			}
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x005: // (8XY4) VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
			if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) {
				V[0xF] = 0;
			} else {
				V[0xF] = 1;
			}
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x006: // (8XY6) Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
			V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0x01);
			V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8] >> 1);
			pc += 2;
			break;

		case 0x007: // (8XY7) Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
			if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
				V[0xF] = 0;
			} else {
				V[0xF] = 1;
			}
			V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8]);
			pc += 2;
			break;

		case 0x000E: // (8XYE) Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
			V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0x80);
			V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8] << 1);
			pc += 2;
			break;
		}
		break;

	case 0x9000: // (9XY0) Skips the next instruction if VX doesn't equal VY.
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
			pc += 4;
		} else {
			pc += 2;
		}
		break;

	case 0xA000: // (ANNN) Sets I to the address NNN.
		I = (opcode & 0x0FFF);
		pc += 2;
		break;

	case 0xB000: // (BNNN) 	Jumps to the address NNN plus V0.
		pc = (V[0x0] + (opcode & 0x0FFF));
		break;

	case 0xC000: // (CXNN) Sets VX to the result of a bitwise and operation on a random number
		V[(opcode & 0x0F00) >> 8] = ((rand() % 256) & (opcode & 0x00FF));
		pc += 2;
		break;

	case 0xD000: // (DXYN) Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
		drawSprite();
		break;
	
	case 0xE000: // There are multiple cases for a leading E
		switch (opcode & 0x000F) {
		case 0x000E: // (EX9E) Skips the next instruction if the key stored in VX is pressed.
			if (key[V[(opcode & 0x0F00) >> 8]] != 0) {
				pc += 4;
			} else {
				pc += 2;
			}
			break;

		case 0x0001: // (EXA1) Skips the next instruction if the key stored in VX isn't pressed.
			if (key[V[(opcode & 0x0F00) >> 8]] == 0) {
				pc += 4;
			} else {
				pc += 2;
			}
			break;
		}
		break;

	case 0xF000: // There are multiple cases for a leading F
		switch (opcode & 0x00FF) { // F uses the whole last byte for opcodes
		case 0x0007: // (FX07) Sets VX to the value of the delay timer.
			V[(opcode & 0x0F00) >> 8] = delay_timer;
			pc += 2;
			break;

		case 0x000A: // (FX0A) A key press is awaited, and then stored in VX.
			// DO THIS
			pc += 2;
			break;

		case 0x0015: // (FX15) Sets the delay timer to VX.
			delay_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x0018: // (FX18) Sets the sound timer to VX.
			sound_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x001E: // (FX1E) Adds VX to I
			I += V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x0029: // (FX29) Sets I to the location of the sprite for the character in VX.
			I = 0x050 + (V[(opcode & 0x0F00) >> 8] * 5); // 0x050 is where the fontset begins, 5 is the length of a character.
			pc += 2;
			break;

		case 0x0033: // (FX33) Sets I to the BCD of VX (see wiki)
		{
			unsigned short x = V[(opcode & 0x0F00) >> 8];
			memory[I] = x / 100;
			memory[I + 1] = (x / 10) % 10;
			memory[I + 2] = (x % 100) % 10;
			pc += 2;
			break;
		}

		case 0x0055: // (FX55) Stores V0 to VX (including VX) in memory starting at address I.
		{
			unsigned short x = ((opcode & 0x0F00) >> 8);
			for (int i = 0; i <= x; i++) {
				memory[I + i] = V[i];
			}
			pc += 2;
			break;
		}

		case 0x0065: // (FX65) Fills V0 to VX (including VX) with values from memory starting at address I.
		{
			unsigned short x = ((opcode & 0x0F00) >> 8);
			for (int i = 0; i <= x; i++) {
				V[i] = memory[I + i];
			}
			pc += 2;
			break;
		}
		}
		break;

	default:
		printf("Unknown opcode: 0x%X\n", opcode);
	}

	// Update timers
	if (delay_timer > 0) {
		delay_timer--;
	}

	if (sound_timer > 0) {
		sound_timer--;
		printf("BEEEEEP\n");
	}
}

void Chip8::drawSprite() {
	unsigned short x = V[(opcode & 0x0F00) >> 8];
	unsigned short y = V[(opcode & 0x00F0) >> 4];
	unsigned short height = (opcode & 0x000F);
	unsigned short pixel;

	V[0xF] = 0;

	for (int yline = 0; yline < height; yline++) {
		pixel = memory[I + yline];
		for (int xline = 0; xline < 8; xline++) {
			if ((pixel & (0x80 >> xline)) != 0) {
				if (gfx[(x + xline + ((y + yline) * 64))] == 1) {
					V[0xF] = 1;
				}
				gfx[(x + xline + ((y + yline) * 64))] ^= 1;
			}
		}
	}

	drawFlag = true;
	pc += 2;
}

void Chip8::clearScreen() {
	for (int i = 0; i < sizeof(gfx); i++) {
		gfx[i] = 0;
		drawFlag = true;
	}
}

unsigned char* Chip8::getGfx() {
	return gfx;
}

void Chip8::loadFontset() {

	unsigned char chip8_fontset[80] = {
			0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
			0x20, 0x60, 0x20, 0x20, 0x70, // 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
			0x90, 0x90, 0xF0, 0x10, 0x10, // 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
			0xF0, 0x10, 0x20, 0x40, 0x40, // 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
			0xF0, 0x90, 0xF0, 0x90, 0x90, // A
			0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
			0xF0, 0x80, 0x80, 0x80, 0xF0, // C
			0xE0, 0x90, 0x90, 0x90, 0xE0, // D
			0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
			0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	for (int i = 0; i < 80; i++) { // Fontset is 80 bytes long
		memory[i + 0x050] = chip8_fontset[i]; // Font loads in starting at 0x050
	}
}