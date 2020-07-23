#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "SDL.h"
#include "glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader {

		unsigned int ID_;

	public:

		Shader() {}

		Shader(const char* vertexPath, const char* fragmentPath) {
			std::string vertexCode;
			std::string fragmentCode;
			std::ifstream vShaderFile;
			std::ifstream fShaderFile;

			vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

			try {
				vShaderFile.open(vertexPath);
				fShaderFile.open(fragmentPath);
				std::stringstream vShaderStream; 
				std::stringstream fShaderStream;

				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();

				vShaderFile.close();
				fShaderFile.close();

				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();
			}
			catch (std::ifstream::failure& e) {
				std::cout << "Shader-Daten wurden nicht erfolgreich eingelesen!" << std::endl;
			}
			const char* vShaderCode = vertexCode.c_str();
			const char* fShaderCode = fragmentCode.c_str();

			unsigned int vertex, fragment;

			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);
			checkCompileErrors(vertex, "Vertex");

			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);
			checkCompileErrors(fragment, "Fragment");

			ID_ = glCreateProgram();
			glAttachShader(ID_, vertex);
			glAttachShader(ID_, fragment);

			glLinkProgram(ID_);
			checkCompileErrors(ID_, "Program");

			glDeleteShader(vertex);
			glDeleteShader(fragment);
		}

		void use() {
			glUseProgram(ID_);
		}

		void erase() {
			glDeleteProgram(ID_);
		}

		void setTransform(std::string name, glm::mat4 matrix) {
			glUniformMatrix4fv(glGetUniformLocation(ID_, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
		}

		void setView(std::string name, glm::mat4 matrix) {
			glUniformMatrix4fv(glGetUniformLocation(ID_, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
		}

		void setInt(std::string name, int value) {
			glUniform1i(glGetUniformLocation(ID_, name.c_str()), value);
		}

		void setVec3(std::string name, glm::vec3 value) {
			glUniform3fv(glGetUniformLocation(ID_, name.c_str()), 1, &value[0]);
		}

		void setProjection(std::string name, glm::mat4 matrix) {
			glUniformMatrix4fv(glGetUniformLocation(ID_, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
		}

	private:

		void checkCompileErrors(GLuint shader, std::string type)
		{
			GLint success;
			GLchar infoLog[1024];
			if (type != "Program")
			{
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				}
			}
			else
			{
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success)
				{
					glGetProgramInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				}
			}
		}
};
#endif