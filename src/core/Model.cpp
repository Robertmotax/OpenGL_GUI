#include "core/Model.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
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

    glm::mat4 model = getModelMatrix();
    GLuint shaderID = shader->getID();

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uVP"), 1, GL_FALSE, glm::value_ptr(viewProj));

    // --- Texture 2D binding ---
    if (useTexture && texture) {
        glUniform1i(glGetUniformLocation(shaderID, "uUseTexture"), 1);
        texture->useTexture();
        glUniform1i(glGetUniformLocation(shaderID, "uTexture"), 0);
    } else {
        glUniform1i(glGetUniformLocation(shaderID, "uUseTexture"), 0);
    }
    glUniform1i(glGetUniformLocation(shaderID, "uIsUnlit"), getIsUnlit() ? 1 : 0);
    
    // Start from stored position
    glm::mat4 modelTransformed = glm::translate(model, position);

    // --- Shadow cubemap lights ---
    if (!getIsUnlit())
    {
        glUniform1i(glGetUniformLocation(shaderID, "uNumLights"), (int)lights.size());

        for (int i = 0; i < (int)lights.size(); ++i) {
            const LightSource& light = lights[i];
            std::string idx = std::to_string(light.id);

            glUniform3fv(glGetUniformLocation(shaderID, ("lightPositions[" + idx + "]").c_str()), 1, glm::value_ptr(*light.position));
            glUniform3fv(glGetUniformLocation(shaderID, ("lightColors[" + idx + "]").c_str()), 1, glm::value_ptr(*light.color));
            glUniform1f(glGetUniformLocation(shaderID, ("lightIntensities[" + idx + "]").c_str()), length(*light.intensity));
            glUniform1f(glGetUniformLocation(shaderID, ("farPlanes[" + idx + "]").c_str()), light.farPlane);

            glActiveTexture(GL_TEXTURE1 + i);
            glBindTexture(GL_TEXTURE_CUBE_MAP, light.getShadowCubemap());
            glUniform1i(glGetUniformLocation(shaderID, ("shadowMaps[" + idx + "]").c_str()), 1 + i);
        }
    } else {
        glUniform1i(glGetUniformLocation(shaderID, "uNumLights"), 0);
    }

    //Bind new data 
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Optional: debug OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
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

/**
 * Load the model from the given path using OBJLoaderV2.
 * Returns true if successful, false otherwise.
 *  @brief make sure that vertexIndices is int and not GLuint
 */
bool Model::setupModelEBO(const char* path) {
    //use the predefined OBJLoaderV2
    return loadOBJ2(path, vertexIndices, temp_vertices, out_normals, out_uvs);
}

/**
 * // Uncomment this if you want to use Assimp instead of OBJLoaderV2
 */
// bool Model::setupModelEBO(const std::string& path) {
//     Assimp::Importer importer;

//     const aiScene* scene = importer.ReadFile(path,
//         aiProcess_Triangulate |
//         aiProcess_GenSmoothNormals |
//         aiProcess_CalcTangentSpace |
//         aiProcess_JoinIdenticalVertices |
//         aiProcess_ImproveCacheLocality |
//         aiProcess_OptimizeMeshes);

//     if (!scene || !scene->HasMeshes()) {
//         std::cerr << "Assimp load error: " << importer.GetErrorString() << std::endl;
//         return false;
//     }

//     const aiMesh* mesh = scene->mMeshes[0]; // Use the first mesh

//     std::vector<Vertex> vertices;
//     std::vector<unsigned int> indices;

//     for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
//         Vertex vertex{};

//         // Position
//         vertex.position = {
//             mesh->mVertices[i].x,
//             mesh->mVertices[i].y,
//             mesh->mVertices[i].z
//         };

//         // Normal
//         if (mesh->HasNormals()) {
//             vertex.normal = {
//                 mesh->mNormals[i].x,
//                 mesh->mNormals[i].y,
//                 mesh->mNormals[i].z
//             };
//         }

//         // UV
//         if (mesh->HasTextureCoords(0)) {
//             vertex.texCoord = {
//                 mesh->mTextureCoords[0][i].x,
//                 mesh->mTextureCoords[0][i].y
//             };
//         }

//         vertices.push_back(vertex);
//     }

//     for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
//         const aiFace& face = mesh->mFaces[i];
//         if (face.mNumIndices != 3) continue; // Skip non-triangle faces

//         indices.push_back(face.mIndices[0]);
//         indices.push_back(face.mIndices[1]);
//         indices.push_back(face.mIndices[2]);
//     }

//     // Store in class
//     this->vertexCount = indices.size();
//     this->vertexIndices = indices;
//     this->temp_vertices.clear();
//     this->out_normals.clear();
//     this->out_uvs.clear();

//     for (const auto& v : vertices) {
//         temp_vertices.push_back(v.position);
//         out_normals.push_back(v.normal);
//         out_uvs.push_back(v.texCoord);
//     }

//     return true;
// }