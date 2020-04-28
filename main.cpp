#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SDL.h"


int main(int argc, char** argv) {

	srand(time(NULL));
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Event event;
	bool quit = false;

	int color[7][3] = { {148, 0, 211}, {75, 0, 130}, {0, 0, 255}, {0, 255, 0}, {255, 255, 0}, {255, 127, 0}, {255, 0, 0} };

	int counter = 0;

	int frame_rate = 24;

	while (!quit) {
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) {
			quit = true;
			break;
		}

		SDL_SetRenderDrawColor(renderer, color[counter][0], color[counter][1], color[counter][2], 255);

		SDL_RenderClear(renderer);

		SDL_RenderPresent(renderer);

		
		counter = (counter + 1) % 7;

		SDL_Delay(1000/frame_rate);
		
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}