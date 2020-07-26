#pragma once

#include <string>
#include <vector>
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;
};

struct Texture {
	unsigned int ID;
	std::string type;
	std::string path;
};

class Mesh {
	public:
		std::vector<Vertex>			vertices;
		std::vector<unsigned int>	indices;
		std::vector<Texture>		textures;

		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;

			setupMesh();
		}

		void Draw(Shader& shader) {
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			for (unsigned int i = 0; i < textures.size(); i++) {
				glActiveTexture(GL_TEXTURE0 + i);

				std::string number;
				std::string name = textures[i].type;
				if (name == "texture_diffuse") {
					number = std::to_string(diffuseNr++);
				} else if (name == "texture_specular") {
					number = std::to_string(specularNr++);
				}

				shader.setFloat(("material." + name + number).c_str(), i);
				glBindTexture(GL_TEXTURE_2D, textures[i].ID);
			}
			glActiveTexture(GL_TEXTURE0);

			// Mesh zeichnen
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	
	private:
		unsigned int VAO, VBO, EBO;

		void setupMesh() {
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			// Vertex Position
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

			// Vertex Normale
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

			// Vertex Texturekoordinaten
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));

			glBindVertexArray(0);
		}
};