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
		return false;
	}
	
	//allocate image3d	
	int width = 0;
	int height = 0;
	int depth = 0;
	
	if(nin->dim > 0)
		width = nin->axis[0].size;
	if(nin->dim > 1)
		height = nin->axis[1].size;
	if(nin->dim > 2)
		depth = nin->axis[2].size;
	
	//load nrrd data into image3d
	if(nin->type == nrrdTypeChar || nin->type == nrrdTypeUChar)
	{
		image->Allocate(width, height, depth, 2);
		uint16_t* imdata = (uint16_t*)image->Data(); 
		for(uint64_t i = 0; i < width * height * depth; i++)
		{
			imdata[i] = ((uint8_t*)(nin->data))[i]; 
		}
	}
	else if(nin->type == nrrdTypeShort || nin->type == nrrdTypeUShort)
	{
		image->Allocate(width, height, depth, 2);
		uint16_t* imdata = (uint16_t*)image->Data(); 
		for(uint64_t i = 0; i < width * height * depth; i++)
		{
			imdata[i] = ((uint16_t*)(nin->data))[i]; 
		}
	}
	
	//delete nrrd 
	nrrdNuke(nin);
	return true; 
	
}
