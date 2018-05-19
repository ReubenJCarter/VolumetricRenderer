#include "Image3DFromTIFFFile.hpp"

#include <png.h>

bool Image3DFromPNGFileSequence(Image3D* image, std::vector<std::string> fileNames)
{
	//Check for all same size in stack
	uint32_t width = 0;
	uint32_t height = 0; 
	bool firstRun = true; 
	for(int i = 0; i < fileNames.size(); i++)
	{
		FILE* file = fopen(fileNames[i].c_str(), "rb");
		if(file)
		{
			//check png file is valid png
			char checkBuffer[8];
			fread(checkBuffer, 1, 8, file);
			int isPng = !png_sig_cmp((png_const_bytep)checkBuffer, 0, 8);
			if (!isPng)
			{
				std::cerr << "Image3DFromPNGFileSequence: File could not be read as a png" << fileNames[i] << std::endl;
				return false;
			}
			
			//Create structures
			png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			if(!pngPtr)
			{
				std::cerr << "Image3DFromPNGFileSequence: could not create png_structp" << fileNames[i] << std::endl;	
				return false;
			}

			png_infop infoPtr = png_create_info_struct(pngPtr);
			if (!infoPtr)
			{
                std::cerr << "Image3DFromPNGFileSequence: could not create png_infop" << fileNames[i] << std::endl;	
				return false;
			}

			if (setjmp(png_jmpbuf(pngPtr)))
			{
				std::cerr << "Image3DFromPNGFileSequence: setjmp fail" << fileNames[i] << std::endl;	
				return false;
			}

			//Read the png and just get the w and h
			png_init_io(pngPtr, file);
			png_set_sig_bytes(pngPtr, 8);

			png_read_info(pngPtr, infoPtr);
			
			uint32_t w = png_get_image_width(pngPtr, infoPtr);
			uint32_t h = png_get_image_height(pngPtr, infoPtr);
			
			if (setjmp(png_jmpbuf(pngPtr)))
			{
				std::cerr << "Image3DFromPNGFileSequence: setjmp fail" << fileNames[i] << std::endl;
				return false; 
			}
			
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
					std::cerr << "Image3DFromPNGFileSequence: Images not the same size" << fileNames[i] << std::endl;
					return false; 
				}
			}
			fclose(file);
		}
	}
	
	//Check if images have some wifdth and height
	if(width <= 0 || height <= 0 || fileNames.size() <= 0)
	{
		std::cerr << "Image3DFromPNGFileSequence: Images have zero size" << std::endl;
		return false;
	}
	
	//Allocate Image
	image->Allocate(width, height, fileNames.size(), 4);
	
	//Load Data
	for(int i = 0; i < fileNames.size(); i++)
	{
		FILE* file = fopen(fileNames[i].c_str(), "rb");
		if(file)
		{	
			//check png file is valid png
			char checkBuffer[8];
			fread(checkBuffer, 1, 8, file);
			int isPng = !png_sig_cmp((png_const_bytep)checkBuffer, 0, 8);
			if (!isPng)
			{
				std::cerr << "Image3DFromPNGFileSequence: File could not be read as a png" << fileNames[i] << std::endl;
				return false;
			}
			
			//Create structures
			png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			if(!pngPtr)
			{
				std::cerr << "Image3DFromPNGFileSequence: could not create png_structp" << fileNames[i] << std::endl;	
				return false;
			}

			png_infop infoPtr = png_create_info_struct(pngPtr);
			if (!infoPtr)
			{
                std::cerr << "Image3DFromPNGFileSequence: could not create png_infop" << fileNames[i] << std::endl;	
				return false;
			}

			if (setjmp(png_jmpbuf(pngPtr)))
			{
				std::cerr << "Image3DFromPNGFileSequence: setjmp fail" << fileNames[i] << std::endl;	
				return false;
			}

			//Read the png and just get the w and h
			png_init_io(pngPtr, file);
			png_set_sig_bytes(pngPtr, 8);

			png_read_info(pngPtr, infoPtr);

			png_byte colorType = png_get_color_type(pngPtr, infoPtr);
			png_byte bitDepth = png_get_bit_depth(pngPtr, infoPtr);

			int numberOfPasses = png_set_interlace_handling(pngPtr);
			png_read_update_info(pngPtr, infoPtr);

			if (setjmp(png_jmpbuf(pngPtr)))
			{
				std::cerr << "Image3DFromPNGFileSequence: setjmp fail" << fileNames[i] << std::endl;
				return false; 
			}
			
			unsigned char* imageData = (unsigned char*)image->Data() + width * height * 4 * i;
			
			png_bytep* rowPointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
			for (int y = 0; y < height; y++)
                rowPointers[y] = (png_byte*) malloc(png_get_rowbytes(pngPtr,infoPtr));

			png_read_image(pngPtr, rowPointers);	


			if(png_get_color_type(pngPtr, infoPtr) != PNG_COLOR_TYPE_RGBA)
			{
				std::cerr << "Image3DFromPNGFileSequence: PNG file not of type RGBA" << fileNames[i] << std::endl;
				return false; 
			}
			
			for(int y = 0; y < height; y++) 
			{
                png_byte* row = rowPointers[y];
                for(int x = 0; x < width; x++) 
				{
                        png_byte* ptr = &(row[x*4]);
                       
						imageData[(y * width + x) * 4 + 0] = ptr[0];
						imageData[(y * width + x) * 4 + 1] = ptr[1];
						imageData[(y * width + x) * 4 + 2] = ptr[2];
						imageData[(y * width + x) * 4 + 3] = ptr[3];
                }
			}

			fclose(file);
		}
	}
	
	return true; 
}