#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(const char* vertexPath, const char* fragmentPath, bool isNDC);
    ~Shader();

    void use() const;
    GLuint getID() const;

private:
    GLuint ID;
    bool isNDC;

    const char* loadShaderSource(const char* filepath);
    void setBool(const std::string& name, bool value) const;
};
