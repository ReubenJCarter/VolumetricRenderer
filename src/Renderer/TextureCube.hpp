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
		void LoadDataXPos(void* xPosBuf);
		void LoadDataXNeg(void* xNegBuf);
		void LoadDataYPos(void* yPosBuf);
		void LoadDataYNeg(void* yNegBuf);
		void LoadDataZPos(void* zPosBuf);
		void LoadDataZNeg(void* zNegBuf);
		unsigned int GetTextureId();
		uint64_t Width();
		uint64_t Height();
		uint64_t Depth();
};