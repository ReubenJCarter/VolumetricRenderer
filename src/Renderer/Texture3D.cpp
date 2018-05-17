#include "Texture3D.hpp"


Texture3D::Texture3D()
{
	OPENGL_FUNC_MACRO
	
	ogl->glGenTextures(1, &textureId);
	
	ogl->glBindTexture(GL_TEXTURE_3D, textureId);
	
	ogl->glBindTexture(GL_TEXTURE_3D, 0);
	
	width = 0;
	height = 0;
	depth = 0; 
	channels = 4;
}

void Texture3D::Allocate(uint64_t w, uint64_t h, uint64_t d, bool compressed, int chan)
{
	width = w;
	height = h;
	depth = d; 
	channels = chan;
	
	OPENGL_FUNC_MACRO

	ogl->glBindTexture(GL_TEXTURE_3D, textureId);
	
	int internalFormats[] = {GL_COMPRESSED_RED, GL_COMPRESSED_RG, GL_COMPRESSED_RGB, GL_COMPRESSED_RGBA, GL_R8, GL_RG8, GL_RGB8, GL_RGBA8}; 
	int dataFormats[] = {GL_RED, GL_RG, GL_RGB, GL_RGBA}; 
	
	int internalFormat = compressed ? internalFormats[chan-1] : internalFormats[chan-1 + 4];	
	int dataFormat = dataFormats[chan-1]; 
	
	ogl->glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, height, depth, 0, dataFormat, GL_UNSIGNED_BYTE, NULL);
	
	ogl->glBindTexture(GL_TEXTURE_3D, 0);
}

void Texture3D::Destroy()
{
	OPENGL_FUNC_MACRO

	ogl->glDeleteTextures(1, &textureId);
}

void Texture3D::LoadData(void* buffer)
{
	OPENGL_FUNC_MACRO

	int dataFormats[] = {GL_RED, GL_RG, GL_RGB, GL_RGBA}; 
	
	ogl->glBindTexture(GL_TEXTURE_3D, textureId);
	ogl->glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width, height, depth, dataFormats[channels-1], GL_UNSIGNED_BYTE, buffer);
	ogl->glBindTexture(GL_TEXTURE_3D, 0);
}

void Texture3D::LoadDataSlice(void* buffer, uint64_t Z)
{
	OPENGL_FUNC_MACRO

	int dataFormats[] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};
	
	ogl->glBindTexture(GL_TEXTURE_3D, textureId);
	ogl->glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, Z, width, height, 1, dataFormats[channels-1], GL_UNSIGNED_BYTE, buffer);

	ogl->glBindTexture(GL_TEXTURE_3D, 0);
}

unsigned int Texture3D::GetTextureId()
{
	return textureId; 
}

uint64_t Texture3D::Width()
{
	return width; 
}

uint64_t Texture3D::Height()
{
	return height;
}

uint64_t Texture3D::Depth()
{
	return depth; 
}
