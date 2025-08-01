#pragma once
#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "core/RenderableObject.h"
#include "core/Shader.h"

class Model: public RenderableObject {
public:
    Model(const std::string& path, Shader* shader, Shader* shaderShadow);
    ~Model();

    void draw(const glm::mat4& viewProj, const std::vector<LightSource>& lights) const;
    int getVertexCount() const { return vertexCount; }
    GLuint getVAO() const { return VAO; }

private:
    //bool setupModelEBO(const char* path);
    bool setupModelEBO(const std::string& path);
    void setupBuffers();

    std::string filepath;
    int vertexCount;

    std::vector<GLuint> vertexIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> out_normals;
    std::vector<glm::vec2> out_uvs;

    GLuint VAO; //vertex array object
    GLuint VBO_vertices; //vertex buffer object
    GLuint VBO_normals; //lighting
    GLuint VBO_uvs; //texture
    GLuint EBO; //element buffer object for proper fast loading
};
