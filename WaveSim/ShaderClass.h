#pragma once

//Don't define this if it's already defined
#ifndef SHADER_H
#define SHADER_H

//glad
#include <glad/glad.h>

//For reading from files:
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
	unsigned int ID;
	//Constructor generates the shader program.
	Shader(const char* vertexPath, const char* fragmentPath) {
		std::string vCode;
		std::string fCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		//Make sure the ifstreams can throw exceptions
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
		
		//Read the vertex and fragment shader files, and put the text into vCode and fCode respectively
		try {
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			vShaderFile.close();
			fShaderFile.close();
			vCode = vShaderStream.str();
			fCode = fShaderStream.str();
		}

		//Handle the error if the shader file can't be read
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR: shader file not successfully read" << std::endl;
		}

		//Change the shader code into c strings
		const char* vShaderCode = vCode.c_str();
		const char* fShaderCode = fCode.c_str();

		//Compile shaders
		unsigned int vertex, fragment;
		//Vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "vertex shader");
		//Fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "fragment shader");
		//Shader program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "shader program");
		//Delete shaders
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	//To use the shader program:
	void use() {
		glUseProgram(ID);
	}

	//Setting uniforms for the shaders
	void setBool(const std::string &name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setInt(const std::string &name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string &name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

private:
	void checkCompileErrors(unsigned int shader, std::string type) {
		int success;
		char infoLog[1024];
		if (type != "shader program") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR: shader compilation error for " << type << "\n" << infoLog << "\n" << std::endl;
			}
		}
		else {
			glGetProgramiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR: shader program linking error for " << type << "\n" << infoLog << "\n" << std::endl;
			}
		}
	}
};

#endif