#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#include "SDL.h"
#include "glew.h"


int main(int argc, char** argv) {

	srand(time(NULL));

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4.1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4.6);
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	GLboolean glewExperimental = GL_TRUE;
	GLenum glewInit();

	SDL_Event event;
	bool quit = false;

	int colors[3] = {0, 0, 0};

	while (!quit) {
		// Events
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				quit = true;
				break;
			}
			else{
				switch (event.key.keysym.sym) {
					case SDLK_q:
						quit = true;
						break;
					case SDLK_r:
						colors[0] = 1;
						colors[1] = 0;
						colors[2] = 0;
						break;
					case SDLK_g:
						colors[0] = 0;
						colors[1] = 1;
						colors[2] = 0;
						break;
					case SDLK_b:
						colors[0] = 0;
						colors[1] = 0;
						colors[2] = 1;
						break;
					case SDLK_w:
						colors[0] = 1;
						colors[1] = 1;
						colors[2] = 1;
						break;
					case SDLK_l:
						colors[0] = 0;
						colors[1] = 0;
						colors[2] = 0;
						break;
				}
			}
		}

		// leere Fenster
		glClearColor(colors[0], colors[1], colors[2], 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Rendering

		// tausche Puffer
		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}