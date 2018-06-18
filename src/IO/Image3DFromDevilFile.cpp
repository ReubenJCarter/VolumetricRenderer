#include "Image3DFromDevilFile.hpp"

#include <IL/il.h>
#include <IL/ilu.h>

bool ILInited = false; 

bool Image3DFromDevilFileSequence(Image3D* image, std::vector<std::string> fileNames)
{
	if(!ILInited)
	{
		ilInit();
		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
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
					ilBindImage(0);
					ilDeleteImages(1, &ilIm);
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
			uint16_t* imageData = (uint16_t*)image->Data() + width * height * i;
			ilConvertImage(IL_LUMINANCE, IL_SHORT); 
			uint16_t* dataPtr = (uint16_t*)ilGetData();
			for(unsigned int i = 0; i < width * height; i++)
			{
				imageData[i] = dataPtr[i];
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
		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
		ILInited = true;
	}
	
	 
	//Check for all same size in stack
	uint32_t width = 0;
	uint32_t height = 0; 
	uint32_t depth = 0; 
	
	ILuint ilIm;
	ilGenImages(1, &ilIm);
	ilBindImage(ilIm);
	bool loadedImageOK = ilLoadImage(fileName.c_str());
	if(loadedImageOK)
	{
		//get image info
		width = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);
		depth = ilGetInteger(IL_IMAGE_DEPTH);
	}
	else
	{
		ilBindImage(0);
		ilDeleteImages(1, &ilIm);
		
		std::cerr << "Image3DFromDevilFile: Image Data not loaded" << std::endl;
		return false;
	}
	
	
	if(width <= 0 || height <= 0 || depth <=0)
	{
		ilBindImage(0);
		ilDeleteImages(1, &ilIm);
		
		std::cerr << "Image3DFromDevilFile: Images have zero size" << std::endl;
		return false;
	}
	
	//Allocate Image
	image->Allocate(width, height, depth, 2);
	
	uint16_t* imageData = (uint16_t*)image->Data(); 
	ilConvertImage(IL_LUMINANCE, IL_SHORT); 
	uint16_t* dataPtr = (uint16_t*)ilGetData();
	for(int k = 0; k < width * height * depth; k++)
	{
		imageData[k] = dataPtr[k];
	}	
	
	ilBindImage(0);
	ilDeleteImages(1, &ilIm);


	return true; 
	
	
}
