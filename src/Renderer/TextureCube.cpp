#include "TextureCube.hpp"


TextureCube::TextureCube()
{
	OPENGL_FUNC_MACRO
	
	ogl->glGenTextures(1, &textureId);
	
	ogl->glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	
	ogl->glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	width = 0;
	height = 0;
	channels = 4;
}

void TextureCube::Allocate(uint64_t w, uint64_t h, bool compressed, int chan)
{
	width = w;
	height = h;
	channels = chan;
	
	OPENGL_FUNC_MACRO

	ogl->glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	
	int internalFormats[] = {GL_COMPRESSED_RED, GL_COMPRESSED_RG, GL_COMPRESSED_RGB, GL_COMPRESSED_RGBA, GL_R8, GL_RG8, GL_RGB8, GL_RGBA8}; 
	int dataFormats[] = {GL_RED, GL_RG, GL_RGB, GL_RGBA}; 
	
	int internalFormat = compressed ? internalFormats[chan-1] : internalFormats[chan-1 + 4];	
	int dataFormat = dataFormats[chan-1]; 
	
	ogl->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, NULL);
	ogl->glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, NULL);
	ogl->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, NULL);
	ogl->glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, NULL);
	ogl->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, NULL);
	ogl->glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, NULL);

	ogl->glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void TextureCube::Destroy()
{
	OPENGL_FUNC_MACRO

	ogl->glDeleteTextures(1, &textureId);
}

void TextureCube::LoadData(void* xPosBuf, void* xNegBuf, void* yPosBuf, void* yNegBuf, void* zPosBuf, void* zNegBuf)
{
	OPENGL_FUNC_MACRO

	int dataFormats[] = {GL_RED, GL_RG, GL_RGB, GL_RGBA}; 
	
	ogl->glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	ogl->glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, width, height, dataFormats[channels-1], GL_UNSIGNED_BYTE, xPosBuf);
	ogl->glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0, 0, width, height, dataFormats[channels-1], GL_UNSIGNED_BYTE, xNegBuf);
	ogl->glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0, 0, width, height, dataFormats[channels-1], GL_UNSIGNED_BYTE, yPosBuf);
	ogl->glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 0, 0, width, height, dataFormats[channels-1], GL_UNSIGNED_BYTE, yNegBuf);
	ogl->glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0, 0, width, height, dataFormats[channels-1], GL_UNSIGNED_BYTE, zPosBuf);
	ogl->glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0, 0, width, height, dataFormats[channels-1], GL_UNSIGNED_BYTE, zNegBuf);

	ogl->glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}


unsigned int TextureCube::GetTextureId()
{
	return textureId; 
}

uint64_t TextureCube::Width()
{
	return width; 
}

uint64_t TextureCube::Height()
{
	return height;
}

uint64_t TextureCube::Depth()
{
	return depth; 
}
