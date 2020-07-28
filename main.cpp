#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <ctime>

#include "Model.h"
#include "Shader.h"
#include "Camera.h"
#include "stb_image.h"

#include "SDL.h"
#include "glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

const float ScreenWidth = 1280;
const float ScreenHeight = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, -3.0f));
float lastX = ScreenWidth / 2.0f;
float lastY = ScreenHeight / 2.0f;

// Shader
Shader ourShader;
Shader skyboxShader;

SDL_Window* window;
SDL_Event event;
glm::mat4 transform_matrix;
glm::mat4 projection_matrix;
glm::mat4 view_matrix;

void eventHandler(bool* quit) {
	const float cameraSpeed = 0.1f;
	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			*quit = true;
			break;
		}
		else if (event.type == SDL_MOUSEMOTION) {
			camera.ProcessMouseMovement(event.motion.xrel, event.motion.yrel);
		}
		else if (event.type == SDL_MOUSEWHEEL) {
			camera.ProcessMouseScroll(event.motion.x);
		}
		else {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				*quit = true;
				break;
			// Rotation
			case SDLK_KP_8:
				transform_matrix = glm::rotate(transform_matrix, 0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_KP_4:
				transform_matrix = glm::rotate(transform_matrix, -0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_KP_5:
				transform_matrix = glm::rotate(transform_matrix, -0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_KP_6:
				transform_matrix = glm::rotate(transform_matrix, 0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_KP_7:
				transform_matrix = glm::rotate(transform_matrix, -0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
				ourShader.setTransform("transform", transform_matrix);
				break;
			case SDLK_KP_9:
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
			//Kamera (über Numpad-Pfeile steuerbar)
			case SDLK_w:
				camera.ProcessKeyBoard(FORWARD);
				break;
			case SDLK_s:
				camera.ProcessKeyBoard(BACK);
				break;
			case SDLK_a:
				camera.ProcessKeyBoard(LEFT);
				break;
			case SDLK_d:
				camera.ProcessKeyBoard(RIGHT);
				break;
			}
		}
	}
}

void calcNormals(float vertices[], int size, int floats_per_triangle, int floats_per_vertex) {
	std::vector<glm::vec3> normals;
	glm::vec3 point_a;
	glm::vec3 point_b;
	glm::vec3 point_c;
	glm::vec3 vec1;
	glm::vec3 vec2;
	glm::vec3 normal;

	for (int i = 0; i < size; i++) {
		point_a = glm::vec3(vertices[0 + i * floats_per_triangle], vertices[1 + i * floats_per_triangle], vertices[2 + i * floats_per_triangle]);
		point_b = glm::vec3(vertices[0 + i * floats_per_triangle + floats_per_vertex], vertices[1 + i * floats_per_triangle + floats_per_vertex], vertices[2 + i * floats_per_triangle + floats_per_vertex]);
		point_c = glm::vec3(vertices[0 + i * floats_per_triangle + 2 * floats_per_vertex], vertices[1 + i * floats_per_triangle + 2 * floats_per_vertex], vertices[2 + i * floats_per_triangle + 2 * floats_per_vertex]);

		vec1 = point_b - point_a;
		vec2 = point_c - point_a;

		normal = glm::normalize(glm::cross(vec1, vec2));
		normals.push_back(normal);
	}

	// Wert der Normalen setzen
	int offset = 6;
	for (int i = 0; i < normals.size(); i++) { // für jedes Dreieck
		for (int c = 0; c < 3; c++) { // jede Normale pro Dreieck für jeden Vertex
			vertices[c * floats_per_vertex + i * floats_per_triangle + offset] = normals.at(i).x;
			vertices[c * floats_per_vertex + i * floats_per_triangle + offset + 1] = normals.at(i).y;
			vertices[c * floats_per_vertex + i * floats_per_triangle + offset + 2] = normals.at(i).z;
		}
	}
}

unsigned int loadCubemap(std::vector<std::string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	unsigned char* data;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Laden von Cubemap Texture bei: " << faces[i] << " nicht geladen!" << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

unsigned int loadTexture(char const* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1) {
			format = GL_RED;
		}
		else if (nrComponents == 3) {
			format = GL_RGB;
		}
		else if (nrComponents == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture konnte nicht geladen werden von: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

int main(int argc, char** argv) {

	SDL_Init(SDL_INIT_EVERYTHING);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	window = SDL_CreateWindow("Computer Grafik", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenWidth, ScreenHeight, SDL_WINDOW_OPENGL);
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	GLboolean glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK){
		std::cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
		exit(EXIT_FAILURE);
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);

	float vertices[]{
		// Position			|	Farbe			   |	Normalen				| Texturekoordinaten
		// erstes Dreieck
	/*0*/ -0.6f, -0.6f,  0.39f,	1.0f,  0.0f,  0.0f,		0.0f,  0.0f,  0.0f,		-0.6f, -0.6f, 
	/*1*/  0.6f, -0.6f,  0.39f,	0.0f,  1.0f,  0.0f,		0.0f,  0.0f,  0.0f,		 0.6f, -0.6f,  
	/*2*/  0.0f, -0.6f, -0.78f,	0.0f,  0.0f,  1.0f,		0.0f,  0.0f,  0.0f,		 0.0f,  0.6f,  

		 // zweites Dreieck
	/*1*/  0.6f, -0.6f, 0.39f,	1.0f, 0.0f, 0.0f,		0.0f,  0.0f,  0.0f,		 0.6f, -0.6f, 
	/*2*/  0.0f, -0.6f,-0.78f,	0.0f, 1.0f, 0.0f,		0.0f,  0.0f,  0.0f,		 0.0f, -0.6f,
	/*3*/  0.0f,  0.6f,  0.0f,	0.0f, 0.0f, 1.0f,		0.0f,  0.0f,  0.0f,		 0.0f,  0.6f,  

		// drittes Dreieck
	/*0*/ -0.6f, -0.6f, 0.39f,	1.0f, 0.0f, 0.0f,		0.0f,  0.0f,  0.0f,		-0.6f, -0.6f,
	/*1*/  0.6f, -0.6f, 0.39f,	0.0f, 1.0f, 0.0f,		0.0f,  0.0f,  0.0f,		 0.6f, -0.6f,
	/*3*/  0.0f,  0.6f,  0.0f,	0.0f, 0.0f, 1.0f,		0.0f,  0.0f,  0.0f,		 0.0f,  0.6f,  

		// viertes Dreieck
	/*0*/ -0.6f, -0.6f, 0.39f,	1.0f, 0.0f, 0.0f,		0.0f,  0.0f,  0.0f,		-0.6f, -0.6f, 
	/*2*/  0.0f, -0.6f,-0.78f,	0.0f, 1.0f, 0.0f,		0.0f,  0.0f,  0.0f,		 0.0f, -0.6f,
	/*3*/  0.0f,  0.6f,  0.0f,	0.0f, 0.0f, 1.0f,		0.0f,  0.0f,  0.0f,		 0.0f,  0.6f
	};

	float skyboxVertices[] = {
		// Position          
		-50.0f,  50.0f, -50.0f,
		-50.0f, -50.0f, -50.0f,
		 50.0f, -50.0f, -50.0f,
		 50.0f, -50.0f, -50.0f,
		 50.0f,  50.0f, -50.0f,
		-50.0f,  50.0f, -50.0f,

		-50.0f, -50.0f,  50.0f,
		-50.0f, -50.0f, -50.0f,
		-50.0f,  50.0f, -50.0f,
		-50.0f,  50.0f, -50.0f,
		-50.0f,  50.0f,  50.0f,
		-50.0f, -50.0f,  50.0f,

		 50.0f, -50.0f, -50.0f,
		 50.0f, -50.0f,  50.0f,
		 50.0f,  50.0f,  50.0f,
		 50.0f,  50.0f,  50.0f,
		 50.0f,  50.0f, -50.0f,
		 50.0f, -50.0f, -50.0f,

		-50.0f, -50.0f,  50.0f,
		-50.0f,  50.0f,  50.0f,
		 50.0f,  50.0f,  50.0f,
		 50.0f,  50.0f,  50.0f,
		 50.0f, -50.0f,  50.0f,
		-50.0f, -50.0f,  50.0f,

		-50.0f,  50.0f, -50.0f,
		 50.0f,  50.0f, -50.0f,
		 50.0f,  50.0f,  50.0f,
		 50.0f,  50.0f,  50.0f,
		-50.0f,  50.0f,  50.0f,
		-50.0f,  50.0f, -50.0f,

		-50.0f, -50.0f, -50.0f,
		-50.0f, -50.0f,  50.0f,
		 50.0f, -50.0f, -50.0f,
		 50.0f, -50.0f, -50.0f,
		-50.0f, -50.0f,  50.0f,
		 50.0f, -50.0f,  50.0f
	};

	ourShader = Shader("Vertex.txt", "Fragment.txt");
	skyboxShader = Shader("skybox_vertex.txt", "skybox_fragment.txt");

	// Texturen
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set texture wrapping options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering options 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load Image
	int width, height, nrChannels;
	unsigned char* data = stbi_load("moneyprintergobrrr.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture!" << std::endl;
	}
	stbi_image_free(data);

	// Normalen berechnen
	int size = sizeof(vertices) / (sizeof(float) * 33); // 36 Floats pro Dreieck
	calcNormals(vertices, size, 33, 11);

	// VAO, VBO erstellen
	GLuint VBO, VAO;
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// VAO, VBO, IBO binden/aktivieren
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// VBO füllen
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Vertex Positionen einem Array-Index zuordnen
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// Farbwerte
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Normalen
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Texturen
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// Transformationsmatrix, als Einheitsmatrix initialisiert
	transform_matrix = glm::mat4(1.0f);
	view_matrix = glm::mat4(1.0f);
	projection_matrix = glm::mat4(1.0f);

	// --------------------------------------------------------------------------------------------------
	// Skybox
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	std::vector<std::string> faces{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	
	ourShader.use();
	ourShader.setInt("skybox", 0);
	ourShader.setInt("ourTexture", 1);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);
	// Render Loop
	bool quit = false;

	//Model ourModel("resources/objects/backpack/backpack.obj");

	while (!quit) {

		// Events
		eventHandler(&quit);
		
		// leere Fenster
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glDepthFunc(GL_LESS);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projection_matrix = glm::perspective(glm::radians(camera.Zoom_), ScreenWidth/ScreenHeight, 0.1f, 100.0f);
		ourShader.use();
		ourShader.setTransform("transform", transform_matrix);
		ourShader.setProjection("projection", projection_matrix);
		ourShader.setView("view", camera.getViewMatrix());
		ourShader.setVec3("cameraPos", camera.getPosition());
		ourShader.setVec3("light.position", glm::vec3(0.0f, 50.0f, 0.0f));
		ourShader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		ourShader.setVec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
		ourShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

		ourShader.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
		ourShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
		ourShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		ourShader.setFloat("material.shininess", 1000.0f);

		// Rendering
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawArrays(GL_TRIANGLES, 0, 12);
		glBindVertexArray(0);
		//ourModel.Draw(ourShader);

		// Skybox
		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
		skyboxShader.setView("view",view);
		skyboxShader.setProjection("projection", projection_matrix);

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);


		// tausche Puffer
		SDL_GL_SwapWindow(window);
	}

	//glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	//glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &skyboxVBO);
	ourShader.erase();
	skyboxShader.erase();

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}