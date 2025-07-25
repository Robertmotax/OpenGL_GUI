#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

class Shader {
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    GLuint getID() const;
    void use() const;

    void setVec3(const std::string& name, const glm::vec3& value) {
        GLint loc = glGetUniformLocation(shaderID, name.c_str());
        if (loc != -1)
            glUniform3fv(loc, 1, glm::value_ptr(value));
    }

    void setFloat(const std::string& name, float value) {
        GLint loc = glGetUniformLocation(shaderID, name.c_str());
        if (loc != -1)
            glUniform1f(loc, value);
    }

    void setMat4(const std::string& name, const glm::mat4& mat) {
        GLint loc = glGetUniformLocation(shaderID, name.c_str());
        if (loc != -1)
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    }


    void setInt(const std::string& name, int value) {
        GLint loc = glGetUniformLocation(shaderID, name.c_str());
        if (loc != -1)
            glUniform1i(loc, value);
    }

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
