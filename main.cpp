#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#include "SDL.h"
#include "glew.h"

SDL_Window* window;

void createShader(GLenum type, GLuint* id, const std::string code) {

	GLint status = false;

	const char* code_c_str = code.c_str();
	
	*id = glCreateShader(type);
	
	glShaderSource(*id, 1, &code_c_str, NULL);
	glCompileShader(*id);
	glGetShaderiv(*id, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		std::cout << "Vertex-Shader wurde nicht kompiliert" << std::endl;
	}
}

void createShaderProgram(GLuint* shader_id, GLuint* program_id) {
	*program_id = glCreateProgram();

	glAttachShader(*program_id, *shader_id);

	glLinkProgram(*program_id);

	glUseProgram(*program_id);
}

int main(int argc, char** argv) {

	srand(time(NULL));

	SDL_Init(SDL_INIT_EVERYTHING);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4.1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4.6);
	window = SDL_CreateWindow("Computer Grafik", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	GLboolean glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK){
		std::cout << "F" << std::endl;
		exit(EXIT_FAILURE);
	}

	SDL_Event event;
	bool quit = false;

	int colors[3] = {0, 0, 0};

	/* Geometrie
		C++ Geometry-Klasse			
		OpenGL Vertex Array Object	

		Platz:	
		std::vector
		Vertex Buffer Object

		Dreiecke erzeugen:	
		AddTriangle-Methode in Geometry-Klasse
		glBufferData(.),glVertexAttribPointer(.)

		Renderen:
		Render-Methode in Geometry-Klasse
		glBindvertexArray(.),glDrawArrays(.)
	
	*/

	float vertices[]{
		-0.5f, -0.5f,  0.0f,
		 0.0f,  0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f
	};

	GLuint VBO, VAO;
	
	// VAO, VBO erstellen
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// VAO, VBO binden/aktivieren
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// VBO füllen
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Vertex Daten einem Array-Indes zuordnen
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	// Shader
	GLuint v_shader, f_shader;

	// Vertex-Shader
	const std::string v_sh_code = "# version 330 core \n layout(location = 0) in vec3 vertex_position; void main(){ gl_Position.xyz = vertex_position; gl_Position.w = 1.0;}";
	createShader(GL_VERTEX_SHADER, &v_shader, v_sh_code);

	// Fragment-Shader
	const std::string f_sh_code ="#version 330 core \n layout(location = 0) out vec3 color; void main(){ color = vec3(0.443, 0.694, 0.153);}";
	createShader(GL_FRAGMENT_SHADER, &f_shader, f_sh_code);
	
	// Shader-Programme erzeugen
	GLuint v_sh_pro, f_sh_pro;
	
	createShaderProgram(&v_shader, &v_sh_pro);
	createShaderProgram(&f_shader, &f_sh_pro);

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
					/*case SDLK_r:
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
						break;*/
				}
			}
		}

		// leere Fenster
		glClearColor(colors[0], colors[1], colors[2], 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Rendering
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// tausche Puffer
		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}