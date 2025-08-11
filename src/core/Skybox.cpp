#include <core/Skybox.h>
#include <iostream>

/**
 * @brief Constructor for Skybox, initializes the cubemap texture and sets up the shader.
 * @param faces A vector of strings containing the paths to the six cubemap textures.
 * @param shader The shader to be used for rendering the skybox.
 */
Skybox::Skybox(Shader* shader, Shader* shaderShadow)
    : RenderableObject(generateCubeBoxEncapsulated(), shader, shaderShadow, false)
{
    name = "Skybox";
    useTexture = false; // We'll handle cubemap texture binding manually
}

Skybox::~Skybox() {}


void Skybox::draw(const glm::mat4& viewProj, const std::vector<LightSource*>& lights, glm::vec3 cameraPos) const
{
    // Use less or equal depth test so skybox is drawn behind everything else
    glDepthFunc(GL_LEQUAL);

    shader->use();

    glm::mat4 model = getModelMatrix();

    // Remove translation from view matrix so skybox stays centered on camera
    GLuint shaderID = shader->getID();

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "uVP"), 1, GL_FALSE, glm::value_ptr(viewProj));
    glUniform3fv(glGetUniformLocation(shaderID, "uViewPos"), 1, glm::value_ptr(cameraPos));

    shader->setInt("skybox", 0);  // Make sure your shader uses "skybox" samplerCube uniform

    // --- Texture 2D binding ---
    glUniform1i(glGetUniformLocation(shaderID, "uUseTexture"), 1);
    texture->useTexture(true);
    glUniform1i(glGetUniformLocation(shaderID, "uTexture"), 0);

    // Draw the cube mesh
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    // Restore default depth func
    glDepthFunc(GL_LESS);
}

std::vector<Tri> Skybox::generateCubeBoxEncapsulated() {
    std::vector<Tri> tris;
    // Scale factor
    const float s = 10.0f;

    // Define 24 vertices with position, normal, texCoord, and color (white)
    Vertex vertices[24] = {
        // Front face
        { {-s, -s,  s}, {1,1,1}, {0, 0}, { 0,  0,  1} },
        { { s, -s,  s}, {1,1,1}, {1, 0}, { 0,  0,  1} },
        { { s,  s,  s}, {1,1,1}, {1, 1}, { 0,  0,  1} },
        { {-s,  s,  s}, {1,1,1}, {0, 1}, { 0,  0,  1} },

        // Back face
        { { s, -s, -s}, {1,1,1}, {0, 0}, { 0,  0, -1} },
        { {-s, -s, -s}, {1,1,1}, {1, 0}, { 0,  0, -1} },
        { {-s,  s, -s}, {1,1,1}, {1, 1}, { 0,  0, -1} },
        { { s,  s, -s}, {1,1,1}, {0, 1}, { 0,  0, -1} },

        // Left face
        { {-s, -s, -s}, {1,1,1}, {0, 0}, {-1,  0,  0} },
        { {-s, -s,  s}, {1,1,1}, {1, 0}, {-1,  0,  0} },
        { {-s,  s,  s}, {1,1,1}, {1, 1}, {-1,  0,  0} },
        { {-s,  s, -s}, {1,1,1}, {0, 1}, {-1,  0,  0} },

        // Right face
        { { s, -s,  s}, {1,1,1}, {0, 0}, { 1,  0,  0} },
        { { s, -s, -s}, {1,1,1}, {1, 0}, { 1,  0,  0} },
        { { s,  s, -s}, {1,1,1}, {1, 1}, { 1,  0,  0} },
        { { s,  s,  s}, {1,1,1}, {0, 1}, { 1,  0,  0} },

        // Top face
        { {-s,  s,  s}, {1,1,1}, {0, 0}, { 0,  1,  0} },
        { { s,  s,  s}, {1,1,1}, {1, 0}, { 0,  1,  0} },
        { { s,  s, -s}, {1,1,1}, {1, 1}, { 0,  1,  0} },
        { {-s,  s, -s}, {1,1,1}, {0, 1}, { 0,  1,  0} },

        // Bottom face
        { {-s, -s, -s}, {1,1,1}, {0, 0}, { 0, -1,  0} },
        { { s, -s, -s}, {1,1,1}, {1, 0}, { 0, -1,  0} },
        { { s, -s,  s}, {1,1,1}, {1, 1}, { 0, -1,  0} },
        { {-s, -s,  s}, {1,1,1}, {0, 1}, { 0, -1,  0} },
    };

    for (int face = 0; face < 6; ++face) {
        int idx = face * 4;
        tris.push_back(Tri(vertices[idx], vertices[idx + 1], vertices[idx + 2]));
        tris.push_back(Tri(vertices[idx], vertices[idx + 2], vertices[idx + 3]));
    }

    return tris;
}