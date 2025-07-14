#include "core/Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>


Shader::Shader(const char* vertexPath, const char* fragmentPath, bool isNDC) {
    const char* vertexShaderSource = loadShaderSource(vertexPath);
    const char* fragmentShaderSource = loadShaderSource(fragmentPath);

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    ID = shaderProgram;
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

void Shader::use() const {
    glUseProgram(ID);
    setBool("isNDC", isNDC);
}

GLuint Shader::getID() const {
    return ID;
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
    return buffer; // ✅ Caller owns memory, must delete[]
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
