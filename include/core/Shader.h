#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    GLuint getID() const;
    void use() const;

private:
    GLuint shaderID;
    const char* loadShaderSource(const char* filepath);

    // Compile the shader from source code
    void compileShader(const char* vertexCode, const char* fragmentCode);
    // Add shader to the program
	void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
    //read vertex and fragment shader files and compile them
    const char* readShaderSourceFile(const char* filepath);
};
