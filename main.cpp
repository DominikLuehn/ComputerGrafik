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
// camera
Camera camera(glm::vec3(-21.4649982f, 12.9599619f, 23.56644207f));
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
				ourShader.setMat4("transform", transform_matrix);
				break;
			case SDLK_KP_4:
				transform_matrix = glm::rotate(transform_matrix, -0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
				ourShader.setMat4("transform", transform_matrix);
				break;
			case SDLK_KP_5:
				transform_matrix = glm::rotate(transform_matrix, -0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
				ourShader.setMat4("transform", transform_matrix);
				break;
			case SDLK_KP_6:
				transform_matrix = glm::rotate(transform_matrix, 0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
				ourShader.setMat4("transform", transform_matrix);
				break;
			case SDLK_KP_7:
				transform_matrix = glm::rotate(transform_matrix, -0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
				ourShader.setMat4("transform", transform_matrix);
				break;
			case SDLK_KP_9:
				transform_matrix = glm::rotate(transform_matrix, 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
				ourShader.setMat4("transform", transform_matrix);
				break;
			// Translation
			case SDLK_UP:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(0.0f, 0.1f, 0.0f));
				ourShader.setMat4("transform", transform_matrix);
				break;
			case SDLK_LEFT:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(0.1f, 0.0f, 0.0f));
				ourShader.setMat4("transform", transform_matrix);
				break;
			case SDLK_DOWN:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(0.0f, -0.1f, 0.0f));
				ourShader.setMat4("transform", transform_matrix);
				break;
			case SDLK_RIGHT:
				transform_matrix = glm::translate(transform_matrix, glm::vec3(-0.1f, 0.0f, 0.0f));
				ourShader.setMat4("transform", transform_matrix);
				break;
			// Skalierung
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
				ourShader.setMat4("transform", transform_matrix);
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
	for (int i = 0; i < normals.size(); i++) { // f�r jedes Dreieck
		for (int c = 0; c < 3; c++) { // jede Normale pro Dreieck f�r jeden Vertex
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

void fps()
{
	static int fps = 0;
	static float before = 0.0f;
	static char strFPS[20] = { 0 };
	static float now = (GetTickCount() * 0.001f);

	++fps;

	if ((now - before) > 1.0f)
	{
		before = now;
		sprintf_s(strFPS, "FPS: %c", int(fps));
		SDL_SetWindowTitle(window, strFPS);
		fps = 0;
		
	}
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

	ourShader = Shader("Vertex.txt", "Fragment.fs");
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
	
	ourShader.use();
	ourShader.setInt("skybox", 0);
	ourShader.setInt("ourTexture", 1);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);
	// Render Loop
	bool quit = false;

	Model ourModel("resources/objects/tobitlabs.obj");
	Model boden("resources/objects/boden.obj");

	// Instancing
	//glm::mat4 translations[9];
	//int index = 0;
	//float offset = 5.0f;
	//for (int z = 0; z < 3; z++) {
	//	for (int x = 0; x < 3; x++) {
	//		glm::mat4 translation = glm::mat4(1.0f);
	//		glm::vec3 displace;
	//		displace.x = x * offset;
	//		displace.y = 0.0f;
	//		displace.z = z * offset;
	//		translation = glm::translate(translation, displace);
	//		translations[index++] = translation;
	//	}
	//}

	while (!quit) {

		// Events
		eventHandler(&quit);
		fps();
		// leere Fenster
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glDepthFunc(GL_LESS);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projection_matrix = glm::perspective(glm::radians(camera.Zoom_), ScreenWidth/ScreenHeight, 0.1f, 100.0f);
		ourShader.use();
		ourShader.setMat4("transform", transform_matrix);
		ourShader.setProjection("projection", projection_matrix);
		ourShader.setMat4("view", camera.getViewMatrix());
		ourShader.setVec3("cameraPos", camera.getPosition());
		ourShader.setVec3("light.position", glm::vec3(0.0f, 50.0f, 0.0f));
		ourShader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		ourShader.setVec3("light.diffuse", glm::vec3(0.3f, 0.3f, 0.3f));
		ourShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

		ourShader.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
		ourShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
		ourShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		ourShader.setFloat("material.shininess", 1.0f);


		// Shift color more to blue if neccessary
		if (changeColor == BLUE && (currentColor.x != 0.0f || currentColor.y != 0.0f || currentColor.z != 0.5f)) {
			if (currentColor.x != 0.0f) {
				if (currentColor.x < 0.0f) {
					currentColor.x += 0.001f;
				} else if  (currentColor.x > 0.0f) {
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
			std::ostringstream spotlightBase;
			spotlightBase << "spotLights["<< 15 + i;

			std::ostringstream spotlightPos;
			spotlightPos << spotlightBase.str() << "].position";
			ourShader.setVec3(spotlightPos.str(), glm::vec3(i, 8.33154964f, 9.46781199f));


			std::ostringstream spotlightDir;
			spotlightDir << spotlightBase.str() << "].direction";
			ourShader.setVec3(spotlightDir.str(), glm::vec3(0.0f, -1.0f, -0.25f));
			
			if (changeColor == BUNT) {
				switch ((i+15) % 3) {
				case 0: currentColor = glm::vec3(0.5f, 0.0f, 0.0f); break;
				case 1: currentColor = glm::vec3(0.0f, 0.5f, 0.0f); break;
				case 2: currentColor = glm::vec3(0.0f, 0.0f, 0.5f); break;
				}
			}

			std::ostringstream spotlightDif;
			spotlightDif << spotlightBase.str() << "].diffuse";
			ourShader.setVec3(spotlightDif.str() , currentColor);
			
			std::ostringstream spotlightSpec;
			spotlightSpec<< spotlightBase.str() << "].specular";
			ourShader.setVec3("spotLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));

			std::ostringstream spotlightAmbient;
			spotlightAmbient << spotlightBase.str();
			ourShader.setVec3(spotlightSpec.str(), glm::vec3(0.0f, 0.0f, 0.0f));

			std::ostringstream spotlightCutOff;
			spotlightCutOff << spotlightBase.str() << "].cutOff";
			ourShader.setFloat(spotlightCutOff.str(), glm::cos(glm::radians(12.5f)));

			std::ostringstream spotlightOuterCutOff;
			spotlightOuterCutOff << spotlightBase.str() << "].outerCutOff";
			ourShader.setFloat(spotlightOuterCutOff.str(), glm::cos(glm::radians(17.5f)));
		}
		
		// set lightning for left side to building
		for (int i = 18; i < 51; i++) {
			std::ostringstream spotlightBase;
			spotlightBase << "spotLights[" << 15 + i;

			std::ostringstream spotlightPos;
			spotlightPos << spotlightBase.str() << "].position";
			ourShader.setVec3(spotlightPos.str(), glm::vec3(i - 33, 8.33154964f, -7.4f));


			std::ostringstream spotlightDir;
			spotlightDir << spotlightBase.str() << "].direction";
			ourShader.setVec3(spotlightDir.str(), glm::vec3(0.0f, -1.0f, 0.25f));

			if (changeColor == BUNT) {
				switch (i % 3) {
				case 0: currentColor = glm::vec3(0.5f, 0.0f, 0.0f); break;
				case 1: currentColor = glm::vec3(0.0f, 0.5f, 0.0f); break;
				case 2: currentColor = glm::vec3(0.0f, 0.0f, 0.5f); break;
				}
			}

			std::ostringstream spotlightDif;
			spotlightDif << spotlightBase.str() << "].diffuse";
			ourShader.setVec3(spotlightDif.str(), currentColor);

			std::ostringstream spotlightSpec;
			spotlightSpec << spotlightBase.str() << "].specular";
			ourShader.setVec3("spotLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));

			std::ostringstream spotlightAmbient;
			spotlightAmbient << spotlightBase.str();
			ourShader.setVec3(spotlightSpec.str(), glm::vec3(0.0f, 0.0f, 0.0f));

			std::ostringstream spotlightCutOff;
			spotlightCutOff << spotlightBase.str() << "].cutOff";
			ourShader.setFloat(spotlightCutOff.str(), glm::cos(glm::radians(12.5f)));

			std::ostringstream spotlightOuterCutOff;
			spotlightOuterCutOff << spotlightBase.str() << "].outerCutOff";
			ourShader.setFloat(spotlightOuterCutOff.str(), glm::cos(glm::radians(17.5f)));
		}

		// Rendering
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		//for (unsigned int i = 0; i < 9; i++)
		//{
		//	ourShader.setMat4("transform", translations[i]);
			ourModel.Draw(ourShader);
			boden.Draw(ourShader);
//		}

		// Skybox
		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
		skyboxShader.setMat4("view",view);
		skyboxShader.setProjection("projection", projection_matrix);

		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glActiveTexture(GL_TEXTURE0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);


		// tausche Puffer
		SDL_GL_SwapWindow(window);
	}

	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	ourShader.erase();
	skyboxShader.erase();

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}