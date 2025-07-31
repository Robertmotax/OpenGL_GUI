#include "core/Model.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
// Update the path below if OBJloaderV2.h is located elsewhere in your project
#include "../external/OBJloaderV2.h"  //For loading .obj files using a polygon list format

Model::Model(const std::string& path, Shader* shader, Shader* shaderShadow)
    : RenderableObject({}, shader, shaderShadow), //parent class
        filepath(path), vertexCount(0), VAO(0), VBO_vertices(0), VBO_normals(0), VBO_uvs(0), EBO(0)
{
    //attempting to load the object's model directly in the class
    if (setupModelEBO(filepath.c_str())) {
        setupBuffers();
    } else {
        std::cerr << "Failed to load model: " << path << std::endl;
    }
}

Model::~Model() {
    glDeleteBuffers(1, &VBO_vertices);
    glDeleteBuffers(1, &VBO_normals);
    glDeleteBuffers(1, &VBO_uvs);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

//draw the model into the scene 3D
void Model::draw(const glm::mat4& viewProj, const std::vector<LightSource>& lights) const {
    shader->use();
    shader->setMat4("uViewProj", viewProj);

    //Bind new data 
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Model::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // VBOs
    glGenBuffers(1, &VBO_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
    glBufferData(GL_ARRAY_BUFFER, temp_vertices.size() * sizeof(glm::vec3), &temp_vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    if (!out_normals.empty()) {
        glGenBuffers(1, &VBO_normals);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
        glBufferData(GL_ARRAY_BUFFER, out_normals.size() * sizeof(glm::vec3), &out_normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
        glEnableVertexAttribArray(1);
    }

    if (!out_uvs.empty()) {
        glGenBuffers(1, &VBO_uvs);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_uvs);
        glBufferData(GL_ARRAY_BUFFER, out_uvs.size() * sizeof(glm::vec2), &out_uvs[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);
        glEnableVertexAttribArray(2);
    }

    // EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(int), &vertexIndices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);  //unbind

    vertexCount = static_cast<int>(vertexIndices.size());
}

bool Model::setupModelEBO(const char* path) {
    //use the predefined OBJLoaderV2
    return loadOBJ2(path, vertexIndices, temp_vertices, out_normals, out_uvs);
}
