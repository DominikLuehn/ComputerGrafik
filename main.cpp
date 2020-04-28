#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SDL.h"

void change_color(int data[], int size) {
	for (int i = 0; i < size; i++) {
		data[i] = rand() % 256;
	}
}

int main(int argc, char** argv) {

	srand(time(NULL));
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Event event;
	bool quit = false;

	int color[3] = {0, 0, 0};

	while (!quit) {
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) {
			quit = true;
			break;
		}

		SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 255);

		SDL_RenderClear(renderer);

		SDL_RenderPresent(renderer);

		change_color(color, 3);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}