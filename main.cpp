#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <ctime>

#include "windows.h"
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

enum CHANGECOLOR {
	BLUE = 0,
	RED,
	GREEN,
	BUNT
};
glm::vec3 currentColor = glm::vec3(0.0f, 0.0f, 0.5f);
CHANGECOLOR changeColor = BLUE;
float skybox_reflectivity = 0.1f;

// camera
Camera camera(glm::vec3(0.0f, 10.0, 0.0));
float lastX = ScreenWidth / 2.0f;
float lastY = ScreenHeight / 2.0f;

// Shader
Shader shader;
Shader skyboxShader;
boolean dirlight = true;
boolean skybox = true;
boolean spotlight = true;

SDL_Window* window;
SDL_Event event;
glm::mat4 transform_matrix;
glm::mat4 projection_matrix;
glm::mat4 view_matrix;

void eventHandler(bool* quit) {
	const float cameraSpeed = 0.2f;
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
		else if(event.type == SDL_KEYDOWN){
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				*quit = true;
				break;

			case SDLK_F1:
				dirlight = !dirlight;
				shader.use();
				shader.setInt("dir_light", dirlight);
				std::cout << "DirLight: " << std::to_string(dirlight) << "\nSkybox: " << std::to_string(skybox) << "\nSpotlights: " << std::to_string(spotlight) << std::endl;
				break;
			case SDLK_F2:
				skybox = !skybox;
				shader.use();
				shader.setInt("skybox_bool", skybox);
				std::cout << "DirLight: " << std::to_string(dirlight) << "\nSkybox: " << std::to_string(skybox) << "\nSpotlights: " << std::to_string(spotlight) << std::endl;
				break;
			case SDLK_F3:
				spotlight = !spotlight;
				shader.use();
				shader.setInt("spot_light", spotlight);
				std::cout << "DirLight: " << std::to_string(dirlight) << "\nSkybox: " << std::to_string(skybox) << "\nSpotlights: " << std::to_string(spotlight) << std::endl;
				break;
			// Rotation
			case SDLK_KP_8:
				transform_matrix = glm::rotate(transform_matrix, 0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
				shader.setMat4("transform", transform_matrix);
				break;
			case SDLK_KP_4:
				transform_matrix = glm::rotate(transform_matrix, -0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
				shader.setMat4("transform", transform_matrix);
				break;
			case SDLK_KP_5:
				transform_matrix = glm::rotate(transform_matrix, -0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
				shader.setMat4("transform", transform_matrix);
				break;
			case SDLK_KP_6:
				transform_matrix = glm::rotate(transform_matrix, 0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
				shader.setMat4("transform", transform_matrix);
				break;
			case SDLK_KP_7:
				transform_matrix = glm::rotate(transform_matrix, -0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
				shader.setMat4("transform", transform_matrix);
				break;
			case SDLK_KP_9:
				transform_matrix = glm::rotate(transform_matrix, 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
				shader.setMat4("transform", transform_matrix);
				break;
			// Translation
			case SDLK_UP:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(0.0f, 0.1f, 0.0f));
				shader.setMat4("transform", transform_matrix);
				break;
			case SDLK_LEFT:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(0.1f, 0.0f, 0.0f));
				shader.setMat4("transform", transform_matrix);
				break;
			case SDLK_DOWN:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(0.0f, -0.1f, 0.0f));
				shader.setMat4("transform", transform_matrix);
				break;
			case SDLK_RIGHT:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(-0.1f, 0.0f, 0.0f));
				shader.setMat4("transform", transform_matrix);
				break;
			// Beleuchtung
			case SDLK_r:
				changeColor = RED;
				break;
			case SDLK_g:
				changeColor = GREEN;
				break;
			case SDLK_b:
				changeColor = BLUE;
				break;
			case SDLK_x:
				changeColor = BUNT;
				break;
			case SDLK_c:
				glm::vec3 meinePos = camera.Position_;
				break;
			case SDLK_f:
				transform_matrix = glm::scale(transform_matrix, glm::vec3(0.914f, 0.914f, 0.914f));
				shader.setMat4("transform", transform_matrix);
				break;
			//Kamera (�ber Numpad-Pfeile steuerbar)
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
			case SDLK_KP_MINUS:
				if (skybox_reflectivity > 0.0f) {
					skybox_reflectivity = skybox_reflectivity - 0.05f;
					shader.use();
					shader.setFloat("factor", skybox_reflectivity);
				}
				break;
			case SDLK_KP_PLUS:
				if (skybox_reflectivity < 1.0f) {
					skybox_reflectivity = skybox_reflectivity + 0.05f;
					shader.use();
					shader.setFloat("factor", skybox_reflectivity);
				}
				break;
			}
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

void lighting() {
	// Shift color more to blue if neccessary
	if (changeColor == BLUE && (currentColor.x != 0.0f || currentColor.y != 0.0f || currentColor.z != 0.5f)) {
		if (currentColor.x != 0.0f) {
			if (currentColor.x < 0.0f) {
				currentColor.x += 0.001f;
			}
			else if (currentColor.x > 0.0f) {
				currentColor.x -= 0.001f;
			}
		}
		if (currentColor.y != 0.0f) {
			if (currentColor.y < 0.0f) {
				currentColor.y += 0.001f;
			}
			else if (currentColor.y > 0.0f) {
				currentColor.y -= 0.001f;
			}
		}
		if (currentColor.z != 0.5f) {
			if (currentColor.z < 0.5f) {
				currentColor.z += 0.001f;
			}
			else if (currentColor.z > 0.5f) {
				currentColor.z -= 0.001f;
			}
		}
	}

	// Shift color more to red if neccessary
	if (changeColor == RED && (currentColor.x != 0.5f || currentColor.y != 0.0f || currentColor.z != 0.0f)) {
		if (currentColor.x != 0.5f) {
			if (currentColor.x < 0.5f) {
				currentColor.x += 0.001f;
			}
			else if (currentColor.x > 0.5f) {
				currentColor.x -= 0.001f;
			}
		}
		if (currentColor.y != 0.0f) {
			if (currentColor.y < 0.0f) {
				currentColor.y += 0.001f;
			}
			else if (currentColor.y > 0.0f) {
				currentColor.y -= 0.001f;
			}
		}
		if (currentColor.z != 0.0f) {
			if (currentColor.z < 0.0f) {
				currentColor.z += 0.001f;
			}
			else if (currentColor.z > 0.0f) {
				currentColor.z -= 0.001f;
			}
		}
	}

	// Shift color more to green if neccessary
	if (changeColor == GREEN && (currentColor.x != 0.0f || currentColor.y != 0.5f || currentColor.z != 0.0f)) {
		if (currentColor.x != 0.0f) {
			if (currentColor.x < 0.0f) {
				currentColor.x += 0.001f;
			}
			else if (currentColor.x > 0.0f) {
				currentColor.x -= 0.001f;
			}
		}
		if (currentColor.y != 0.5f) {
			if (currentColor.y < 0.5f) {
				currentColor.y += 0.001f;
			}
			else if (currentColor.y > 0.5f) {
				currentColor.y -= 0.001f;
			}
		}
		if (currentColor.z != 0.0f) {
			if (currentColor.z < 0.0f) {
				currentColor.z += 0.001f;
			}
			else if (currentColor.z > 0.0f) {
				currentColor.z -= 0.001f;
			}
		}
	}

	// set lightning for right side of buildung
	for (int i = -15; i < 18; i++) {
		int id = 15 + i;
		shader.setVec3("spotLights[" + std::to_string(id) + "].position", glm::vec3(i, 8.33154964f, 9.46781199f));
		shader.setVec3("spotLights[" + std::to_string(id) + "].direction", glm::vec3(0.0f, -1.0f, -0.25f));

		if (changeColor == BUNT) {
			switch (i % 3) {
			case 0: currentColor = glm::vec3(0.5f, 0.0f, 0.0f); break;
			case 1: currentColor = glm::vec3(0.0f, 0.5f, 0.0f); break;
			case 2: currentColor = glm::vec3(0.0f, 0.0f, 0.5f); break;
			}
		}

		shader.setVec3("spotLights[" + std::to_string(id) + "].diffuse", currentColor);
		shader.setVec3("spotLights[" + std::to_string(id) + "].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setVec3("spotLights[" + std::to_string(id) + "].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		shader.setFloat("spotLights[" + std::to_string(id) + "].cutOff", glm::cos(glm::radians(12.5f)));
		shader.setFloat("spotLights[" + std::to_string(id) + "].outerCutOff", glm::cos(glm::radians(17.5f)));
		shader.setFloat("spotLights[" + std::to_string(id) + "].constant", 1.0f);
		shader.setFloat("spotLights[" + std::to_string(id) + "].linear", 0.022f);
		shader.setFloat("spotLights[" + std::to_string(id) + "].quadratic", 0.0019f);
	}

	// set lightning for left side to building
	for (int i = 18; i < 51; i++) {
		int id = i + 15;
		shader.setVec3("spotLights[" + std::to_string(id) + "].position", glm::vec3(i - 33, 8.33154964f, -7.4f));
		shader.setVec3("spotLights[" + std::to_string(id) + "].direction", glm::vec3(0.0f, -1.0f, 0.25f));

		if (changeColor == BUNT) {
			switch (i % 3) {
			case 0: currentColor = glm::vec3(0.5f, 0.0f, 0.0f); break;
			case 1: currentColor = glm::vec3(0.0f, 0.5f, 0.0f); break;
			case 2: currentColor = glm::vec3(0.0f, 0.0f, 0.5f); break;
			}
		}

		shader.setVec3("spotLights[" + std::to_string(id) + "].diffuse", currentColor);
		shader.setVec3("spotLights[" + std::to_string(id) + "].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setVec3("spotLights[" + std::to_string(id) + "].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		shader.setFloat("spotLights[" + std::to_string(id) + "].cutOff", glm::cos(glm::radians(12.5f)));
		shader.setFloat("spotLights[" + std::to_string(id) + "].outerCutOff", glm::cos(glm::radians(17.5f)));
		shader.setFloat("spotLights[" + std::to_string(id) + "].constant", 1.0f);
		shader.setFloat("spotLights[" + std::to_string(id) + "].linear", 0.022f);
		shader.setFloat("spotLights[" + std::to_string(id) + "].quadratic", 0.0019f);
	}
}

void updateShader(Shader shader) {
	projection_matrix = glm::perspective(glm::radians(camera.Zoom_), ScreenWidth / ScreenHeight, 1.0f, 10000.0f);
	shader.use();
	shader.setMat4("transform", transform_matrix);
	shader.setProjection("projection", projection_matrix);
	shader.setMat4("view", camera.getViewMatrix());
	shader.setVec3("cameraPos", camera.getPosition());
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

	shader = Shader("Vertex.txt", "Fragment.fs");
	skyboxShader = Shader("skybox_vertex.txt", "skybox_fragment.txt");

	// Transformationsmatrix, als Einheitsmatrix initialisiert
	transform_matrix = glm::mat4(1.0f);
	view_matrix = glm::mat4(1.0f);
	projection_matrix = glm::mat4(1.0f);

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

	shader.use();
	shader.setInt("skybox", 0);
	shader.setSkyBoxReflection(0.1f);

	shader.setInt("dir_light", dirlight);
	shader.setInt("skybox_bool", skybox);
	shader.setInt("spot_light", spotlight);

	shader.setVec3("light.direction", glm::vec3(0.0f, -50.0f, 0.0f));
	shader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader.setVec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
	shader.setVec3("light.specular", glm::vec3(0.4f, 0.4f, 0.4f));

	shader.setVec3("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("material.shininess", 1000.0f);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);
	// Render Loop
	bool quit = false;

	Model Haus("resources/objects/Haus/Haus.obj");

	// Instancing
	/*glm::mat4 translations[25];
	int index = 0;
	float offset = 3.5f;
	for (int z = 0; z < 5; z++) {
		for (int x = 0; x < 5; x++) {
			glm::mat4 translation = glm::mat4(1.0f);
			glm::vec3 displace;
			displace.x = x * offset;
			displace.y = 0.0f;
			displace.z = z * offset;
			translation = glm::translate(translation, displace);
			translations[index++] = translation;
		}
	}*/

	glEnable(GL_CULL_FACE);

	while (!quit) {

		// Events
		eventHandler(&quit);
		// leere Fenster
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glDepthFunc(GL_LESS);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		updateShader(shader);

		lighting();

		glActiveTexture(GL_TEXTURE31);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		shader.setInt("skybox", 31);

		// Rendering
		//Haus.Draw(shader);

		// Skybox
		if (skybox) {
			glDepthFunc(GL_LEQUAL);
			skyboxShader.use();
			glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
			skyboxShader.setMat4("view", view);
			skyboxShader.setProjection("projection", projection_matrix);

			glBindVertexArray(skyboxVAO);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glActiveTexture(GL_TEXTURE0);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS);
		}

		// tausche Puffer
		SDL_GL_SwapWindow(window);
	}

	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	shader.erase();
	skyboxShader.erase();

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}