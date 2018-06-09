#pragma once


#include "../Common.hpp"


class Texture3D
{
	protected:
		unsigned int textureId;
		uint64_t width;
		uint64_t height;
		uint64_t depth;
		int channels;
		int bytesPerSample;
		
	public:
		Texture3D();
		void Allocate(uint64_t w, uint64_t h, uint64_t d, bool compressed=true, int chan=4, int bps=1);
		void Destroy();
		void LoadData(void* buffer);
		void LoadDataSlice(void* buffer, uint64_t Z);
		unsigned int GetTextureId();
		uint64_t Width();
		uint64_t Height();
		uint64_t Depth();
};