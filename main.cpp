#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include "SDL.h"
#include "glew.h"

SDL_Window* window;

int background_color[3] = { 0, 0, 0 };

float object_color[]{
		0.0f, 1.0f, 0.0f
};

SDL_Event event;

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
		// erstes Dreieck
		 0.0f,  0.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 1.0f,  1.0f,  0.0f,

		// zweites Dreieck
		 0.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  1.0f,  0.0f,

		// drittes Dreieck
		 0.0f,  0.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		-1.0f, -1.0f,  0.0f,

		 // viertes Dreieck
		 0.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f, -1.0f,  0.0f
	};

	// VAO, VBOerstellen
	GLuint VBO, VAO;
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// VAO, VBO, IBO binden/aktivieren
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// VBO füllen
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Vertex Daten einem Array-Index zuordnen
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);

	// Shader
	GLuint v_shader, f_shader;

	// Vertex-Shader
	const std::string v_sh_code = "# version 330 core \n layout(location = 0) in vec3 vertex_position; void main(){ gl_Position.xyz = vertex_position; gl_Position.w = 1.0;}";
	createShader(GL_VERTEX_SHADER, &v_shader, v_sh_code);

	// Fragment-Shader
	const std::string f_sh_code ="#version 330 core \n layout(location = 0) out vec3 object_color; uniform vec3 geometry_color; void main(){ object_color = geometry_color;}";
	createShader(GL_FRAGMENT_SHADER, &f_shader, f_sh_code);
	
	// Shader-Programme erzeugen
	GLuint v_sh_pro, f_sh_pro;
	
	createShaderProgram(&v_shader, &v_sh_pro);
	createShaderProgram(&f_shader, &f_sh_pro);

	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

	// Render Loop
	bool quit = false;
	
	while (!quit) {
		// Events
		changeColorUniform(f_sh_pro);
		eventHandler(&quit);
		
		// leere Fenster
		glClearColor(background_color[0], background_color[1], background_color[2], 1);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Rendering
		glUseProgram(v_sh_pro);
		glUseProgram(f_sh_pro);

		//glDrawElements(GL_TRIANGLES, indices.size(), GL_FLOAT, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 6);

		// tausche Puffer
		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}