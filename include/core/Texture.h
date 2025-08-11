#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../external/stb_image.h"

class Texture
{
public:
	Texture();
	Texture(std::string fileLoc, bool tiny = false);

	GLuint getTextureID() const { return textureID; }
	void loadTexture(bool tiny);
	void loadTextureSkyBox(const std::vector<std::string>& faces); // For cubemap textures
	void useTexture(bool isSkybox = false);
	void clearTexture();

	~Texture();
	std::string fileLocation;

private:
	GLuint textureID;
	int width, height, bitDepth;
};



#endif