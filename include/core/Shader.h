#pragma once

#include <string>
#include <GL/glew.h>

class Shader {
public:
    Shader(const char* vertexPath, const char* fragmentPath, bool isNDC);
    ~Shader();

    void use() const;
    GLuint getID() const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    GLuint ID;
    bool isNDC;

    const char* loadShaderSource(const char* filepath);
    void setBool(const std::string& name, bool value) const;
};
