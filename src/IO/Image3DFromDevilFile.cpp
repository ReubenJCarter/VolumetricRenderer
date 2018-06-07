#include "Image3DFromDevilFile.hpp"

#include <IL/il.h>
#include <IL/ilu.h>

bool ILInited = false; 

bool Image3DFromDevilFileSequence(Image3D* image, std::vector<std::string> fileNames)
{
	if(!ILInited)
	{
		ilInit();
		ILInited = true;
	}
	
	//Check for all same size in stack
	uint32_t width = 0;
	uint32_t height = 0; 
	bool firstRun = true; 
	for(int i = 0; i < fileNames.size(); i++)
	{
		ILuint ilIm;
		ilGenImages(1, &ilIm);
		ilBindImage(ilIm);
		bool loadedImageOK = ilLoadImage(fileNames[i].c_str());
		if(loadedImageOK)
		{
			//get image info
			int w = ilGetInteger(IL_IMAGE_WIDTH);
			int h = ilGetInteger(IL_IMAGE_HEIGHT);
		
			if(firstRun)
			{
				width = w;
				height = h;
				firstRun = false; 
			}
			else
			{
				if(w != width || h != height)	
				{
					std::cerr << "Image3DFromDevilFileSequence: Images not the same size" << fileNames[i] << std::endl;
					return false; 
				}
			}
		}
		
		ilBindImage(0);
		ilDeleteImages(1, &ilIm);
	}
	
	//Check if images have some wifdth and height
	if(width <= 0 || height <= 0)
	{
		std::cerr << "Image3DFromDevilFileSequence: Images have zero size" << std::endl;
		return false;
	}
	
	//Allocate Image
	image->Allocate(width, height, fileNames.size(), 2);
	
	//Load Data
	for(int i = 0; i < fileNames.size(); i++)
	{
		ILuint ilIm;
		ilGenImages(1, &ilIm);
		ilBindImage(ilIm);
		bool loadedImageOK = ilLoadImage(fileNames[i].c_str());
		if (loadedImageOK) 
		{
			unsigned char* imageData = (unsigned char*)image->Data() + width * height * 2 * i;
			ilConvertImage(IL_LUMINANCE, IL_SHORT); 
			uint8_t* dataPtr = ilGetData();
			for(unsigned int i = 0; i < width * height * 2; i++)
			{
				((uint8_t*)data)[i] = dataPtr[i];
			}
			
		}
		ilBindImage(0);
		ilDeleteImages(1, &ilIm);
	}
	
	return true; 
}


bool Image3DFromDevilFile(Image3D* image, std::string fileName)
{
	if(!ILInited)
	{
		ilInit();
		ILInited = true;
	}
	
	 
	TIFF* tif = TIFFOpen(fileName.c_str(), "r");
		
	if (!tif) 
	{
		std::cout << "Image3DFromDevilFile:" << "failed to open file" << std::endl; 
		return false; 
	}
	
	uint32_t width = 0;
	uint32_t height = 0; 
	bool firstRun = true;
	
	int depth = 0;
	
	while(TIFFReadDirectory(tif))
	{
	
		int w = ilGetInteger(IL_IMAGE_WIDTH);
		int h = ilGetInteger(IL_IMAGE_HEIGHT);
		int d = ilGetInteger(IL_IMAGE_DEPTH);
		
		if(firstRun)
		{
			width = w;
			height = h;
			firstRun = false; 
		}
		else
		{
			if(w != width || h != height)	
			{
				std::cerr << "Image3DFromDevilFile: Images not the same size" << fileName << std::endl;
				return false; 
			}
		}
		depth++; 
	}
	
	TIFFClose(tif);
	
	
	//Check if images have some wifdth and height
	if(width <= 0 || height <= 0)
	{
		std::cerr << "Image3DFromDevilFile: Images have zero size" << std::endl;
		return false;
	}
	
	//Allocate Image
	image->Allocate(width, height, depth, 4);
	
	//Load Data
	tif = TIFFOpen(fileName.c_str(), "r");
	
	if (!tif) 
	{
		std::cout << "Image3DFromDevilFile:" << "failed to open file second time around" << std::endl; 
		return false; 
	}

	int i = 0;
	while(TIFFReadDirectory(tif))
	{
		unsigned char* imageData = (unsigned char*)image->Data() + width * height * 4 * i;
		
		TIFFReadRGBAImage(tif, width, height, (uint32_t*)imageData, 0);
		for(int k = 0; k < width * height; k++)
			std::cout << (int)(imageData[k * 4 + 0]) << " " << (int)(imageData[k * 4 + 1]) << " " << (int)(imageData[k * 4 + 2]) << " " << (int)(imageData[k * 4 + 3]) << std::endl; 
		
		i++;
	}
	
	TIFFClose(tif);

	
	return true; 
	
	
}
