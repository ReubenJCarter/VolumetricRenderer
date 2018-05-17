#pragma once


#include "../Common.hpp"


class TextureCube
{
	protected:
		unsigned int textureId;
		uint64_t width;
		uint64_t height;
		uint64_t depth;
		int channels;
		
	public:
		TextureCube();
		void Allocate(uint64_t w, uint64_t h, bool compressed=true, int chan=4);
		void Destroy();
		void LoadData(void* xPosBuf, void* xNegBuf, void* yPosBuf, void* yNegBuf, void* zPosBuf, void* zNegBuf);
		unsigned int GetTextureId();
		uint64_t Width();
		uint64_t Height();
		uint64_t Depth();
};