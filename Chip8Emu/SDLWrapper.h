#pragma once
#include <SDL.h>
#include <stdio.h>

class SDLWrapper {
	SDL_Window* window;
	SDL_Surface* screenSurface;
	int screenWidth;
	int screenHeight;
	int upscale;
	unsigned char keys[16];

public:

	// Flags
	bool hasPressedQuit;

	bool init(int width, int height, int up);
	void drawGraphics(unsigned char* gfx);
	unsigned char* getKeyState();
	void shutDown();
};