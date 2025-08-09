#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL\glew.h>
#include <string>

#include "../../external/stb_image.h"

class Texture
{
public:
	Texture();
	Texture(std::string fileLoc, bool tiny = false);

	void loadTexture(bool tiny);
	void useTexture();
	void clearTexture();

	~Texture();
	std::string fileLocation;

private:
	GLuint textureID;
	int width, height, bitDepth;
};



#endif