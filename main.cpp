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
glm::mat4 transform_matrix;
glm::mat4 projection_matrix;
glm::mat4 view_matrix;
glm::vec3 camera_pos;

void eventHandler(bool* quit, float& x_pos, float& z_pos) {
	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			*quit = true;
			break;
		}
		else {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				*quit = true;
				break;
			// Rotation
			case SDLK_w:
				transform_matrix = glm::rotate(transform_matrix, 0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_a:
				transform_matrix = glm::rotate(transform_matrix, -0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_s:
				transform_matrix = glm::rotate(transform_matrix, -0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_d:
				transform_matrix = glm::rotate(transform_matrix, 0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_q:
				transform_matrix = glm::rotate(transform_matrix, -0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_e:
				transform_matrix = glm::rotate(transform_matrix, 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			// Translation
			case SDLK_UP:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(0.0f, 0.1f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_LEFT:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(0.1f, 0.0f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_DOWN:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(0.0f, -0.1f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_RIGHT:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(-0.1f, 0.0f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			// Skalierung
			case SDLK_r:
				transform_matrix = glm::scale(transform_matrix, glm::vec3(1.1f, 1.1f, 1.1f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_f:
				transform_matrix = glm::scale(transform_matrix, glm::vec3(0.914f, 0.914f, 0.914f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			//Kamera (�ber Numpad-Pfeile steuerbar)
			case SDLK_KP_8:
				z_pos += 0.1f;
				break;
			case SDLK_KP_2:
				z_pos += -0.1f;
				break;
			case SDLK_KP_4:
				x_pos += 0.1f;
				break;
			case SDLK_KP_6:
				x_pos -= 0.1f;
				break;
			}
		}
	}
}

std::vector<glm::vec3> calcNormals(float vertices[]) {
	std::vector<glm::vec3> normals;
	glm::vec3 point_a;
	glm::vec3 point_b;
	glm::vec3 point_c;
	glm::vec3 vec1;
	glm::vec3 vec2;
	glm::vec3 normal;

	for (int i = 0; i < 4; i++) {
		point_a = glm::vec3(vertices[0 + i * 27], vertices[1 + i * 27], vertices[2 + i * 27]);
		point_b = glm::vec3(vertices[0 + i * 27 + 9], vertices[1 + i * 27 + 9], vertices[2 + i * 27 + 9]);
		point_c = glm::vec3(vertices[0 + i * 27 + 2 * 9], vertices[1 + i * 27 + 2 * 9], vertices[2 + i * 27 + 2 * 9]);

		std::cout << "Punkt a : " << point_a.x << "|" << point_a.y << "|" << point_a.z << std::endl;
		std::cout << "Punkt b : " << point_b.x << "|" << point_b.y << "|" << point_b.z << std::endl;
		std::cout << "Punkt c : " << point_c.x << "|" << point_c.y << "|" << point_c.z << std::endl;

		vec1 = point_b - point_a;
		vec2 = point_c - point_a;

		std::cout << "Vektor a : " << vec1.x << "|" << vec1.y << "|" << vec1.z << std::endl;
		std::cout << "Vektor b : " << vec2.x << "|" << vec2.y << "|" << vec2.z << std::endl;

		normal = glm::normalize(glm::cross(vec1, vec2));

		std::cout << "Normale : " << normal.x << "|" << normal.y << "|" << normal.z << std::endl;

		normals.push_back(normal);
		std::cout << "----------------------------------------------------" << std::endl;
	}

	return normals;
}

int main(int argc, char** argv) {

	SDL_Init(SDL_INIT_EVERYTHING);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	window = SDL_CreateWindow("Computer Grafik", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	GLboolean glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK){
		std::cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
		exit(EXIT_FAILURE);
	}

	float vertices[]{
		// Normalen wurden vorher gesetzt, da das Abspeichern der berechneten Normalen nicht klappt
		// Position			|	Farbe			   |	Normalen			
		// erstes Dreieck
	/*0*/ -0.6f, -0.6f, 0.39f,	1.0f,  0.0f,  0.0f,		0.0f,  1.0f,  0.0f,
	/*1*/  0.6f, -0.6f, 0.39f,	1.0f,  0.0f,  0.0f,		0.0f,  1.0f,  0.0f,
	/*2*/  0.0f, -0.6f,-0.78f,	1.0f,  0.0f,  0.0f,		0.0f,  1.0f,  0.0f,

		 // zweites Dreieck
	/*1*/  0.6f, -0.6f, 0.39f,	0.0f, 1.0f, 0.0f,		0.853083f,  0.284361f,  -0.437479f,
	/*2*/  0.0f, -0.6f,-0.78f,	0.0f, 1.0f, 0.0f,		0.853083f,  0.284361f,  -0.437479f,
	/*3*/  0.0f,  0.6f,  0.0f,	0.0f, 1.0f, 0.0f,		0.853083f,  0.284361f,  -0.437479f,

		// drittes Dreieck
	/*0*/ -0.6f, -0.6f, 0.39f,	0.0f, 0.0f, 1.0f,		0.0f,  0.309086f,  0.951034f,
	/*1*/  0.6f, -0.6f, 0.39f,	0.0f, 0.0f, 1.0f,		0.0f,  0.309086f,  0.951034f,
	/*3*/  0.0f,  0.6f,  0.0f,	0.0f, 0.0f, 1.0f,		0.0f,  0.309086f,  0.951034f,

		// viertes Dreieck
	/*0*/ -0.6f, -0.6f, 0.39f,	1.0f, 0.0f, 0.0f,		0.853083f,  -0.284361f,  0.437479f,
	/*2*/  0.0f, -0.6f,-0.78f,	0.0f, 1.0f, 0.0f,		0.853083f,  -0.284361f,  0.437479f,
	/*3*/  0.0f,  0.6f,  0.0f,	0.0f, 0.0f, 1.0f,		0.853083f,  -0.284361f,  0.437479f,
	};

	ourShader = Shader("Vertex.txt", "Fragment.txt");

	// Berechnung der Normalen
	int amount;
	amount = (sizeof(vertices) / sizeof(float)) / 27; // 27 Elemente pro Dreieck

	// Normalen berechnen
	std::vector<glm::vec3> normals = calcNormals(vertices);

	for (int i = 0; i < normals.size(); i++) {
		std::cout << "Normale " << i << ": " << normals.at(i).x << "|" << normals.at(i).y << "|" << normals.at(i).z << std::endl;
	}

	// Wert der Normalen setzen
	/*int offset = 6;
	for (int i = 0; i < normals.size(); i++) { // f�r jedes Dreieck
		for (int c = 0; c < 1;) {
			vertices[c * 9 + i * 27 + offset] = normals.at(i).x;
			vertices[c * 9 + i * 27 + offset + 1] = normals.at(i).y;
			vertices[c * 9 + i * 27 + offset + 2] = normals.at(i).z;
		}
	}*/

	// VAO, VBOerstellen
	GLuint VBO, VAO;
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// VAO, VBO, IBO binden/aktivieren
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// VBO f�llen
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
	view_matrix = glm::mat4(1.0f);
	projection_matrix = glm::mat4(1.0f);

	projection_matrix = glm::perspective(glm::radians(60.0f), 1280.0f/720.0f, 0.1f, 100.0f);
	
	ourShader.use();
	ourShader.setProjection("projection", projection_matrix);
	ourShader.setTransform("transform", transform_matrix);

	float x_pos = 0.0f;
	float z_pos = -2.0f;

	// Render Loop
	bool quit = false;

	while (!quit) {
		// Events
		eventHandler(&quit, x_pos, z_pos);
		
		// leere Fenster
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Rendering
		camera_pos = glm::vec3(x_pos, 0.0f, z_pos);
		view_matrix = glm::lookAt(camera_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.use();
		ourShader.setView("view", view_matrix);

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