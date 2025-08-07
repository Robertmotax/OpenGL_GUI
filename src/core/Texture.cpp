#include "core/Texture.h"
#include <iostream>
#include <assert.h>

//default constructor
Texture::Texture() : textureID(0), width(0), height(0), bitDepth(0), fileLocation("")  {}

//main constructor with proper file location
Texture::Texture(const char* fileLoc, bool tiny) 
    : textureID(0), width(0), height(0), bitDepth(0), fileLocation(fileLoc) 
{
    loadTexture(tiny);
}

// read texture file and load it into OpenGL
void Texture::loadTexture(bool tiny) 
{
	// Before loading any textures, call this once:
	stbi_set_flip_vertically_on_load(true);

	unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);
	if (!texData) 
	{
		printf("Failed to find: %s\n", fileLocation);
		return;
	}

	glGenTextures(1, &textureID);
	assert(textureID != 0); // Ensure textureID is valid
	glBindTexture(GL_TEXTURE_2D, textureID);

	//GL_REPEAT is a texture wrapping mode in OpenGL that repeats the texture across
	//the entire surface of a polygon, making the texture coordinates outside the[0, 1]
	//range repeat the texture image instead of clamping to the edge of the texture.
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// mirrored-repeat if go esbeyond eedge of texture, it goes inverted of 1.0f
	// example: if 1.1f, instead of 0.1f it uses 0.9f 
	// // GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

    //upload to GPU
    GLenum format  = 0;
    if (bitDepth == 1) 
        format = GL_RED;
    else if (bitDepth == 3)
        format = GL_RGB;
    else if (bitDepth == 4)
        format = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texData);

	if(!tiny)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(texData);
}

// use the texture in OpenGL
void Texture::useTexture() 
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

// clear the texture from OpenGL
// this is called when the texture is no longer needed
void Texture::clearTexture() 
{
	glDeleteTextures(1, &textureID);
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = "";
}


Texture::~Texture() 
{
    //clearTexture();
}