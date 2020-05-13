#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include "SDL.h"
#include "glew.h"

SDL_Window* window;
SDL_Event event;

int background_color[3] = { 0, 0, 0 };

float object_color[]{
		0.0f, 1.0f, 0.0f
};


void createShader(GLenum type, GLuint* id, const std::string code, int* success) {

	const char* code_c_str = code.c_str();
	
	*id = glCreateShader(type);
	
	glShaderSource(*id, 1, &code_c_str, NULL);
	glCompileShader(*id);
	glGetShaderiv(*id, GL_COMPILE_STATUS, success);
}

void createShaderProgram(GLuint* shader_id, GLuint* shader_id2,GLuint* program_id) {
	int success;

	*program_id = glCreateProgram();

	glAttachShader(*program_id, *shader_id);
	glAttachShader(*program_id, *shader_id2);
	glLinkProgram(*program_id);

	glGetProgramiv(*program_id, GL_LINK_STATUS, &success);

	if (!success) {
		std::cout << "Linking-Fehler!" << std::endl;
	}
}

void changeColorUniform(GLint program) {
	GLuint location;

	location = glGetUniformLocation(program, "geometry_color");

	if (location != -1) {
		glUniform3fv(location, 1, object_color);
	}
	else {
		std::cout << "Uniform Location konnte nicht gefunden werden!." << std::endl;
	}
}

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
			case SDLK_r:
				object_color[0] = 1.0f;
				object_color[1] = 0.0f;
				object_color[2] = 0.0f;
				break;
			case SDLK_g:
				object_color[0] = 0.0f;
				object_color[1] = 1.0f;
				object_color[2] = 0.0f;
				break;
			case SDLK_b:
				object_color[0] = 0.0f;
				object_color[1] = 0.0f;
				object_color[2] = 1.0f;
				break;
			case SDLK_w:
				object_color[0] = 1.0f;
				object_color[1] = 1.0f;
				object_color[2] = 1.0f;
				break;
			case SDLK_l:
				object_color[0] = 0.0f;
				object_color[1] = 0.0f;
				object_color[2] = 0.0f;
				break;
			}
		}
	}
}

int main(int argc, char** argv) {

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

	float vertices[]{
		// Position			|   Farbe
		// erstes Dreieck
		 0.0f, -0.5f,  0.0f,	1.0f, 0.0f, 0.0f,
		 0.0f,  1.0f,  0.0f,	0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f,  0.0f,	0.0f, 0.0f, 1.0f,

		// zweites Dreieck
		 0.5f,  0.0f,  0.5f,	0.0f, 0.0f, 1.0f,
		-1.0f,  0.0f,  0.5f,	0.0f, 0.0f, 1.0f,
		-1.0f,  1.0f,  0.5f,	0.0f, 0.0f, 1.0f,

		// drittes Dreieck 
		 0.0f,  0.5f,  0.9f,	0.0f, 1.0f, 0.0f,
		 0.0f, -1.0f,  0.9f,	0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f,  0.9f,	0.0f, 1.0f, 0.0f,

		 // viertes Dreieck
		-0.5f,  0.0f, -0.5f,	1.0f, 0.0f, 0.0f,
		 1.0f,  0.0f, -0.5f,	1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, -0.5f,	1.0f, 0.0f, 0.0f
	};

	// Shader
	GLuint v_shader, f_shader;
	int success;

	// Vertex-Shader
	const std::string v_sh_code = "#version 330 core \n layout(location = 0) in vec3 vertex_position; layout(location = 1) in vec3 aColor; out vec3 ourColor; void main(){ ourColor = aColor; gl_Position.xyz = vertex_position; gl_Position.w = 1.0;}";
	createShader(GL_VERTEX_SHADER, &v_shader, v_sh_code, &success);

	if (!success) {
		std::cout << "Vertex-Shader wurde nicht kompiliert" << std::endl;
	}

	// Fragment-Shader
	const std::string f_sh_code = "#version 330 core \n out vec4 FragColor; in vec3 ourColor; void main(){ FragColor = vec4(ourColor, 1.0);}";
	createShader(GL_FRAGMENT_SHADER, &f_shader, f_sh_code, &success);

	if (!success) {
		std::cout << "Fragment-Shader wurde nicht kompiliert" << std::endl;
	}

	// Shader-Programme erzeugen
	GLuint shaderProgram;

	createShaderProgram(&v_shader, &f_shader, &shaderProgram);

	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// Farbwerte
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Render Loop
	bool quit = false;
	
	glUseProgram(shaderProgram);

	while (!quit) {
		// Events
		eventHandler(&quit);
		
		// leere Fenster
		glClearColor(background_color[0], background_color[1], background_color[2], 1);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Rendering

		//glDrawElements(GL_TRIANGLES, indices.size(), GL_FLOAT, (void*)0);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 12);

		// tausche Puffer
		SDL_GL_SwapWindow(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}