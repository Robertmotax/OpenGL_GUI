#pragma once
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <core/Shader.h>
#include "core/RenderableObject.h"
#include "core/Texture.h"
#include <core/Tri.h>
#include <core/Vertex.h>
#include <core/Skybox.h>
#include "../../external/stb_image.h"

class Skybox : public RenderableObject
{
public:
    Skybox(Shader* shader, Shader* shaderShadow);
    ~Skybox();

    void draw(const glm::mat4& viewProj, const std::vector<LightSource*>& lights, glm::vec3 cameraPos = glm::vec3(0.0f)) const override;

private:
    Texture* cubeMapTexture = nullptr; // Cubemap texture
    std::vector<std::string> faces;
    std::vector<Tri> generateCubeBoxEncapsulated();
};