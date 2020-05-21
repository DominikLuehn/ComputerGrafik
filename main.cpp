#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include "Shader.h"

#include "SDL.h"
#include "glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

Shader ourShader;
SDL_Window* window;
SDL_Event event;
float background_color[] = { 0.0, 0.0, 0.0 };
float object_color[]{ 0.0f, 1.0f, 0.0f };
glm::mat4 transform_matrix;

void eventHandler(bool* quit) {
	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			*quit = true;
			break;
		}
		else {
			switch (event.key.keysym.sym) {
			case SDLK_q:
				*quit = true;
				break;
			// Rotation
			case SDLK_LEFT:
				transform_matrix = glm::rotate(transform_matrix, 0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_RIGHT:
				transform_matrix = glm::rotate(transform_matrix, -0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_UP:
				transform_matrix = glm::rotate(transform_matrix, 0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_DOWN:
				transform_matrix = glm::rotate(transform_matrix, -0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			// Translation
			case SDLK_w:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(0.0f, 0.1f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_a:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(-0.1f, 0.0f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_s:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(0.0f, -0.1f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_d:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(0.1f, 0.1f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			// Skalierung
			case SDLK_r:
				transform_matrix = glm::scale(transform_matrix, glm::vec3(1.1f, 1.1f, 1.1f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_f:
				transform_matrix = glm::scale(transform_matrix, glm::vec3(0.91f, 0.91f, 0.91f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			}
		}
	}
}

int main(int argc, char** argv) {

	SDL_Init(SDL_INIT_EVERYTHING);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	window = SDL_CreateWindow("Computer Grafik", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_OPENGL);
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	GLboolean glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK){
		std::cout << "F" << std::endl;
		exit(EXIT_FAILURE);
	}

	float vertices[]{
		// Position			|	Farbe			   |	Normalen
		// erstes Dreieck
	/*0*/ -0.6f, -0.6f, 0.39f,	1.0f,  0.0f,  0.0f,		0.0f,  0.0f,  0.0f,
	/*1*/  0.6f, -0.6f, 0.39f,	1.0f,  0.0f,  0.0f,		0.0f,  0.0f,  0.0f,
	/*2*/  0.0f, -0.6f,-0.78f,	1.0f,  0.0f,  0.0f,		0.0f,  0.0f,  0.0f,

		 // zweites Dreieck
	/*1*/  0.6f, -0.6f, 0.39f,	0.0f, 1.0f, 0.0f,		0.0f,  0.0f,  0.0f,
	/*2*/  0.0f, -0.6f,-0.78f,	0.0f, 1.0f, 0.0f,		0.0f,  0.0f,  0.0f,
	/*3*/  0.0f,  0.6f,  0.0f,	0.0f, 1.0f, 0.0f,		0.0f,  0.0f,  0.0f,

		// drittes Dreieck
	/*0*/ -0.6f, -0.6f, 0.39f,	0.0f, 0.0f, 1.0f,		0.0f,  0.0f,  0.0f,
	/*1*/  0.6f, -0.6f, 0.39f,	0.0f, 0.0f, 1.0f,		0.0f,  0.0f,  0.0f,
	/*3*/  0.0f,  0.6f,  0.0f,	0.0f, 0.0f, 1.0f,		0.0f,  0.0f,  0.0f,

		// viertes Dreieck
	/*0*/ -0.6f, -0.6f, 0.39f,	1.0f, 0.0f, 0.0f,		0.0f,  0.0f,  0.0f,
	/*2*/  0.0f, -0.6f,-0.78f,	0.0f, 1.0f, 0.0f,		0.0f,  0.0f,  0.0f,
	/*3*/  0.0f,  0.6f,  0.0f,	0.0f, 0.0f, 1.0f,		0.0f,  0.0f,  0.0f
	};

	ourShader = Shader("Vertex.txt", "Fragment.txt");

	// VAO, VBOerstellen
	GLuint VBO, VAO;
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// VAO, VBO, IBO binden/aktivieren
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// VBO füllen
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Vertex Positionen einem Array-Index zuordnen
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// Farbwerte
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Normalen
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Transformationsmatrix, als Einheitsmatrix initialisiert
	transform_matrix = glm::mat4(1.0f);

	// Render Loop
	bool quit = false;
	
	//glUseProgram(shaderProgram);
	ourShader.use();
	transform_matrix = glm::scale(transform_matrix, glm::vec3(0.5f, 0.5f, 0.5f));
	ourShader.setTransform("transform", transform_matrix);

	while (!quit) {
		// Events
		eventHandler(&quit);
		
		// leere Fenster
		glClearColor(background_color[0], background_color[1], background_color[2], 1);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Rendering
		ourShader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 12);

		// tausche Puffer
		SDL_GL_SwapWindow(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	ourShader.erase();

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}