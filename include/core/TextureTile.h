#pragma once

#include "Texture.h"
#include <GL/glew.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>

/**
 * @file TextureTile.h
 * UNUSED
 */
struct TextureTile {
    GLuint textureID;       // ID of the texture
    Texture* texture; // Pointer to the Texture object
    std::string tileName;   // Name of the texture tile
    std::string textureType; // Type of the texture (e.g., "diffuse", "specular")
};