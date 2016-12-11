#include "SDLWrapper.h"

bool SDLWrapper::init(int width, int height, int up) {
	upscale = up;
	screenWidth = width * upscale;
	screenHeight = height * upscale;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	} else {
		// Create window
		window = SDL_CreateWindow("Chip8Emu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
		if (window == NULL) {
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			return false;
		} else {
			// Get window surface
			screenSurface = SDL_GetWindowSurface(window);
		}
	}

	return true;
}

void SDLWrapper::drawGraphics(unsigned char* gfx) {
	// Fill the screen in black
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));



	for (int i = 0; i < 64 * 32; i++) {
		if (*(gfx + i) == 1) {
			SDL_Rect rect;
			rect.x = ((i * upscale) % screenWidth);
			rect.y = ((i * upscale) / screenWidth) * upscale;
			rect.w = upscale;
			rect.h = upscale;

			SDL_FillRect(screenSurface, &rect, SDL_MapRGB(screenSurface->format, 255, 255, 255));
		}
	}
	SDL_UpdateWindowSurface(window);

}

// Please do not look at the contents of this method
unsigned char* SDLWrapper::getKeyState() {

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			hasPressedQuit = true;
		}

		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) { // ....Just look at the wiki
			case SDLK_1:
				keys[1] = 1;
				break;
			case SDLK_2:
				keys[2] = 1;
				break;
			case SDLK_3:
				keys[3] = 1;
				break;
			case SDLK_q:
				keys[4] = 1;
				break;
			case SDLK_w:
				keys[5] = 1;
				break;
			case SDLK_e:
				keys[6] = 1;
				break;
			case SDLK_a:
				keys[7] = 1;
				break;
			case SDLK_s:
				keys[8] = 1;
				break;
			case SDLK_d:
				keys[9] = 1;
				break;
			case SDLK_z:
				keys[10] = 1;
				break;
			case SDLK_x:
				keys[0] = 1;
				break;
			case SDLK_c:
				keys[11] = 1;
				break;
			case SDLK_4:
				keys[12] = 1;
				break;
			case SDLK_r:
				keys[13] = 1;
				break;
			case SDLK_f:
				keys[14] = 1;
				break;
			case SDLK_v:
				keys[15] = 1;
				break;
			}
		}

		if (event.type == SDL_KEYUP) {
			switch (event.key.keysym.sym) { // ....Just look at the wiki
			case SDLK_1:
				keys[1] = 0;
				break;
			case SDLK_2:
				keys[2] = 0;
				break;
			case SDLK_3:
				keys[3] = 0;
				break;
			case SDLK_q:
				keys[4] = 0;
				break;
			case SDLK_w:
				keys[5] = 0;
				break;
			case SDLK_e:
				keys[6] = 0;
				break;
			case SDLK_a:
				keys[7] = 0;
				break;
			case SDLK_s:
				keys[8] = 0;
				break;
			case SDLK_d:
				keys[9] = 0;
				break;
			case SDLK_z:
				keys[10] = 0;
				break;
			case SDLK_x:
				keys[0] = 0;
				break;
			case SDLK_c:
				keys[11] = 0;
				break;
			case SDLK_4:
				keys[12] = 0;
				break;
			case SDLK_r:
				keys[13] = 0;
				break;
			case SDLK_f:
				keys[14] = 0;
				break;
			case SDLK_v:
				keys[15] = 0;
				break;
			}
		}
	}
	
	return keys;
}

void SDLWrapper::shutDown() {
	SDL_DestroyWindow(window);

	SDL_Quit();
}