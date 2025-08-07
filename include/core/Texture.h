#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL\glew.h>

#include "../../external/stb_image.h"

class Texture
{
public:
	Texture();
	Texture(const char* fileLoc, bool tiny = false);

	void loadTexture(bool tiny);
	void useTexture();
	void clearTexture();

	~Texture();

private:
	GLuint textureID;
	int width, height, bitDepth;

	const char* fileLocation;
};



#endif