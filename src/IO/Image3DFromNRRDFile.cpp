#include "Image3DFromNRRDFile.hpp"

#include <teem/nrrd.h>


bool Image3DFromNRRDFile(Image3D* image, std::string fileName)
{
	// create a nrrd; at this point this is just an empty container 
	Nrrd *nin;
	nin = nrrdNew();
	
	// read in the nrrd from file 
	if(nrrdLoad(nin, fileName.c_str(), NULL))
	{
		char* err = biffGetDone(NRRD);
		std::cout << "Image3DFromNRRDFile: could not load: " << err;
		free(err);
		//delete nrrd 
		nrrdNuke(nin);
		return false;
	}
	
	//allocate image3d	
	uint64_t width = 0;
	uint64_t height = 0;
	uint64_t depth = 0;
	
	if(nin->dim > 0)
		width = nin->axis[0].size;
	if(nin->dim > 1)
		height = nin->axis[1].size;
	if(nin->dim > 2)
		depth = nin->axis[2].size;
	
	
	std::cout << "Image3DFromNRRDFile: Loading image of size: " << width << " x " << height << " x " << depth << std::endl; 
	
	//load nrrd data into image3d
	if(nin->type == nrrdTypeChar || nin->type == nrrdTypeUChar)
	{
		std::cout << "Image3DFromNRRDFile: Data type is Char/uChar" << std::endl; 
		image->Allocate(width, height, depth, 2);
		uint16_t* imdata = (uint16_t*)image->Data(); 
		for(uint64_t i = 0; i < width * height * depth; i++)
		{
			imdata[i] = ((uint8_t*)(nin->data))[i] << 8; 
		}
	}
	else if(nin->type == nrrdTypeUShort)
	{
		std::cout << "Image3DFromNRRDFile: Data type is UShort" << std::endl;
		image->Allocate(width, height, depth, 2);
		uint16_t* imdata = (uint16_t*)image->Data();
		uint64_t inx = 0; 
		for(uint64_t k = 0; k < depth; k++)
		{
			for(uint64_t j = 0; j < height; j++)
			{
				
				for(uint64_t i = 0; i < width; i++)
				{
					imdata[k * width * height + j * width + i] = ((uint16_t*)(nin->data))[inx]; 
					inx++;
				}
			}
		}
	}
	else if(nin->type == nrrdTypeShort)
	{
		std::cout << "Image3DFromNRRDFile: Data type is Short" << std::endl;
		image->Allocate(width, height, depth, 2);
		int16_t* imdata = (int16_t*)image->Data();
		uint64_t inx = 0; 
		for(uint64_t k = 0; k < depth; k++)
		{
			for(uint64_t j = 0; j < height; j++)
			{
				for(uint64_t i = 0; i < width; i++)
				{
					imdata[k * width * height + j * width + i] = (32767 + (int)((int16_t*)(nin->data))[inx]) / 2;
					//std::cout << imdata[k * width * height + j * width + i] << std::endl; 
					inx++;
				}
			}
		}
	}
	else
	{
		std::cout << "Image3DFromNRRDFile: Could not detect supported type of nrrd data " << std::endl; 
		//delete nrrd 
		nrrdNuke(nin);
		return false; 
	}
	
	std::cout << "Image3DFromNRRDFile: Cleanup " << std::endl; 
	
	//delete nrrd 
	nrrdNuke(nin);
	
	std::cout << "Image3DFromNRRDFile: Done Loading image " << std::endl; 
	
	return true; 
	
}
