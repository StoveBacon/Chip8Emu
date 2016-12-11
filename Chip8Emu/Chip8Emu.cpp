#include "SDLWrapper.h"
#include "Chip8.h"

SDLWrapper SDL;
Chip8 chip8;

int main( int argc, char* args[]) {

	SDL.init(64, 32, 10);

	chip8.initialize();
	chip8.loadGame("../Games/breakout");

	while (1) {
		chip8.emulateCycle();

		if (chip8.drawFlag) {
			SDL.drawGraphics(chip8.getGfx());
			chip8.drawFlag = false;
		}

		chip8.setKeys(SDL.getKeyState());
		if (SDL.hasPressedQuit) {
			break;
		}

		SDL_Delay(1.6f);
	}

	SDL.shutDown();

	return 0;
}