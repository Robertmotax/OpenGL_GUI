#include "core/Texture.h"
#include <iostream>
#include <assert.h>
#include <vector>

//default constructor
Texture::Texture() : textureID(0), width(0), height(0), bitDepth(0), fileLocation("")  {}

//main constructor with proper file location
Texture::Texture(std::string fileLoc, bool tiny) 
    : textureID(0), width(0), height(0), bitDepth(0), fileLocation(fileLoc) 
{
    loadTexture(tiny);
}

// read texture file and load it into OpenGL
void Texture::loadTexture(bool tiny) 
{
	// Before loading any textures, call this once:
	stbi_set_flip_vertically_on_load(true);

	unsigned char* texData = stbi_load(fileLocation.c_str(), &width, &height, &bitDepth, 0);
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

/**
 * @brief Load a cubemap texture from a list of file paths.
 * @param faces A vector of strings containing the file paths for each face of the cubemap.
 */
void Texture::loadTextureSkyBox(const std::vector<std::string>& faces) 
{
    assert(faces.size() == 6 && "Cubemap requires exactly 6 face textures");

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false); // cubemap images usually NOT flipped

    for (unsigned int i = 0; i < faces.size(); i++) 
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) 
        {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
            );

            stbi_image_free(data);
        } 
        else 
        {
            std::cerr << "Failed to load cubemap texture at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

// use the texture in OpenGL
void Texture::useTexture(bool isSkybox) 
{
	if (!isSkybox) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
	} else { // for skybox textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	}
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