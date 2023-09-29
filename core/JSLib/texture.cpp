#include "texture.h"
#include "../ew/external/stb_image.h"
#include "../ew/external/glad.h"

unsigned int loadTexture(const char* filePath)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, numComponents;
	unsigned char* data = stbi_load(filePath, &width, &height, &numComponents, 0);
	if (data == NULL)
	{
		printf("Failed to load image %s", filePath);
		stbi_image_free(data);
		return 0;
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	unsigned int glFormat;
	switch (numComponents)
	{
	case 1:
		glFormat = GL_RED;
		break;
	case 2:
		glFormat = GL_RG;
		break;
	case 3:
		glFormat = GL_RGB;
		break;
	default:
		glFormat = GL_RGBA;
		break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}