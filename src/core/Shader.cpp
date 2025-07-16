#include "core/Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

void checkCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // Load from files for scene shader (with lighting)
    char* loadedVertexSource = const_cast<char*>(loadShaderSource(vertexPath));
    char* loadedFragmentSource = const_cast<char*>(loadShaderSource(fragmentPath));
    char* vertexShaderSource = loadedVertexSource;
    char* fragmentShaderSource = loadedFragmentSource;

    if (!vertexShaderSource || !fragmentShaderSource) {
        std::cerr << "ERROR::SHADER::Failed to load shader sources from files." << std::endl;
        return;
    }

    compileShader(vertexShaderSource, fragmentShaderSource);

    // Clean up loaded sources memory if any
    if (loadedVertexSource) {
        delete[] loadedVertexSource;
    }
    if (loadedFragmentSource) {
        delete[] loadedFragmentSource;
    }
}

void Shader::addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}
    // Attach the shader to the program
	glAttachShader(theProgram, theShader);
    // Delete the shader after attaching it to the program
    // This is because the shader code is now part of the program and we don't need it
    glDeleteShader(theShader);
}

void Shader::compileShader(const char* vertexCode, const char* fragmentCode)
{
    shaderID = glCreateProgram();

	if (!shaderID)
	{
		printf("Error creating shader program!\n");
		return;
	}

    //add to the program the specific shader code
	addShader(shaderID, vertexCode, GL_VERTEX_SHADER); // vertex shader
	addShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER); //fragment shader

	GLint result = 0;
	GLchar eLog[1024] = { 0 }; //we use this to acquire the error logs

	glLinkProgram(shaderID);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(shaderID);
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}
}

Shader::~Shader() {
    glDeleteProgram(shaderID);
}

void Shader::use() const {
    glUseProgram(shaderID);
}

GLuint Shader::getID() const {
    return shaderID;
}

const char* Shader::loadShaderSource(const char* filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return nullptr;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[size + 1]; // +1 for null terminator
    if (!file.read(buffer, size)) {
        std::cerr << "Failed to read shader file: " << filepath << std::endl;
        delete[] buffer;
        return nullptr;
    }

    buffer[size] = '\0'; // Null-terminate
    return buffer; // Caller must delete[]
}