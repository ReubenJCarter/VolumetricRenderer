#include "Image3D.hpp"

Image3D::Image3D()
{
	width = 0;
	height = 0; 
	depth = 0;
	pixelSize = 0;
}

Image3D::Image3D(uint64_t W, uint64_t H, uint64_t D, uint64_t P)
{
	Allocate(W, H, D, P);
}

Image3D::~Image3D()
{
	Deallocate();
}

void Image3D::Allocate(uint64_t W, uint64_t H, uint64_t D, uint64_t P)
{
	width = W;
	height = H; 
	depth = D;
	pixelSize = P; 
	data = new unsigned char[W * H * D * P];
}

void Image3D::Deallocate()
{
	if(width > 0 && height > 0 && depth > 0 && pixelSize > 0)
		delete[] (unsigned char*)data; 
}

void* Image3D::Data()
{
	return data; 
}

uint64_t Image3D::Width()
{
	return width; 
}

uint64_t Image3D::Height()
{
	return height; 
}

uint64_t Image3D::Depth()
{
	return depth; 
}

uint64_t Image3D::ByteSize()
{
	return width * height * depth * pixelSize; 
}

void Image3D::Copy(Image3D& inImg)
{
	unsigned char* indata = (unsigned char*)inImg.Data();
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				for(uint64_t c = 0; c < pixelSize; c++)
				{
					((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + c] = 
									indata[(z * width * height + y * width + x) * pixelSize + c];
				}
			}
		}
	}
}

void Image3D::Smooth2D()
{
	if(pixelSize == 1)//8 bit monochrome images
	{
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				int64_t x0 = x+1;
				int64_t x1 = x-1;
				int64_t y0 = y+1;
				int64_t y1 = y-1;
				
				
				int running = 0; 
				
				if(x0 < 0 || x0 >= width) x0 = x;
				if(x1 < 0 || x1 >= width) x1 = x;
				if(y0 < 0 || y0 >= height) y0 = y;
				if(y1 < 0 || y1 >= height) y1 = y;
					
								
				
				running += ((unsigned char*)data)[(z * width * height + y0 * width + x0) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y0 * width + x) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y0 * width + x1) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y * width + x0) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y * width + x1) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y1 * width + x0) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y1 * width + x) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y1 * width + x1) * pixelSize];
				
				
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 0] = running / 27;
				
			}
		}
	}
	}
	else if(pixelSize == 2)//16 bit monochrome images
	{
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				int64_t x0 = x+1;
				int64_t x1 = x-1;
				int64_t y0 = y+1;
				int64_t y1 = y-1;
				
				
				int running = 0; 
				
				if(x0 < 0 || x0 >= width) x0 = x;
				if(x1 < 0 || x1 >= width) x1 = x;
				if(y0 < 0 || y0 >= height) y0 = y;
				if(y1 < 0 || y1 >= height) y1 = y;
					
								
				
				running += ((uint16_t*)data)[(z * width * height + y0 * width + x0)];
				running += ((uint16_t*)data)[(z * width * height + y0 * width + x)];
				running += ((uint16_t*)data)[(z * width * height + y0 * width + x1)];
				running += ((uint16_t*)data)[(z * width * height + y * width + x0)];
				running += ((uint16_t*)data)[(z * width * height + y * width + x)];
				running += ((uint16_t*)data)[(z * width * height + y * width + x1)];
				running += ((uint16_t*)data)[(z * width * height + y1 * width + x0)];
				running += ((uint16_t*)data)[(z * width * height + y1 * width + x)];
				running += ((uint16_t*)data)[(z * width * height + y1 * width + x1)];
				
				
				((uint16_t*)data)[(z * width * height + y * width + x)] = running / 27;
			}
		}
	}	
	}
}

void Image3D::Median2D()
{
	if(pixelSize == 1)//8 bit monochrome images
	{
	std::vector<int> vals(9); 
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				int64_t x0 = x+1;
				int64_t x1 = x-1;
				int64_t y0 = y+1;
				int64_t y1 = y-1;
				
				if(x0 < 0 || x0 >= width) x0 = x;
				if(x1 < 0 || x1 >= width) x1 = x;
				if(y0 < 0 || y0 >= height) y0 = y;
				if(y1 < 0 || y1 >= height) y1 = y;
				
				vals[0] = ((unsigned char*)data)[(z * width * height + y0 * width + x0) * pixelSize];
				vals[1] = ((unsigned char*)data)[(z * width * height + y0 * width + x) * pixelSize];
				vals[2] = ((unsigned char*)data)[(z * width * height + y0 * width + x1) * pixelSize];
				vals[3] = ((unsigned char*)data)[(z * width * height + y * width + x0) * pixelSize];
				vals[4] = ((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize];
				vals[5] = ((unsigned char*)data)[(z * width * height + y * width + x1) * pixelSize];
				vals[6] = ((unsigned char*)data)[(z * width * height + y1 * width + x0) * pixelSize];
				vals[7] = ((unsigned char*)data)[(z * width * height + y1 * width + x) * pixelSize];
				vals[8] = ((unsigned char*)data)[(z * width * height + y1 * width + x1) * pixelSize];
				
				std::nth_element(vals.begin(), vals.begin() + vals.size()/2, vals.end()); 
				
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 0] = vals[vals.size()/2];
				
			}
		}
	}
	}
	if(pixelSize == 2)//16 bit monochrome images
	{
	std::vector<int> vals(9); 
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				int64_t x0 = x+1;
				int64_t x1 = x-1;
				int64_t y0 = y+1;
				int64_t y1 = y-1;
				
				if(x0 < 0 || x0 >= width) x0 = x;
				if(x1 < 0 || x1 >= width) x1 = x;
				if(y0 < 0 || y0 >= height) y0 = y;
				if(y1 < 0 || y1 >= height) y1 = y;				
				
				vals[0] = ((uint16_t*)data)[(z * width * height + y0 * width + x0) ];
				vals[1] = ((uint16_t*)data)[(z * width * height + y0 * width + x) ];
				vals[2] = ((uint16_t*)data)[(z * width * height + y0 * width + x1) ];
				vals[3] = ((uint16_t*)data)[(z * width * height + y * width + x0) ];
				vals[4] = ((uint16_t*)data)[(z * width * height + y * width + x) ];
				vals[5] = ((uint16_t*)data)[(z * width * height + y * width + x1) ];
				vals[6] = ((uint16_t*)data)[(z * width * height + y1 * width + x0) ];
				vals[7] = ((uint16_t*)data)[(z * width * height + y1 * width + x) ];
				vals[8] = ((uint16_t*)data)[(z * width * height + y1 * width + x1) ];
				
				std::nth_element(vals.begin(), vals.begin() + vals.size()/2, vals.end()); 
				
				((uint16_t*)data)[(z * width * height + y * width + x)] = vals[vals.size()/2];
				
			}
		}
	}
	}
}

void Image3D::Smooth()
{
	if(pixelSize == 1)//8 bit monochrome images
	{
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				int64_t x0 = x+1;
				int64_t x1 = x-1;
				int64_t y0 = y+1;
				int64_t y1 = y-1;
				int64_t z0 = z+1;
				int64_t z1 = z-1;
				
				
				int running = 0; 
				
				if(x0 < 0 || x0 >= width) x0 = x;
				if(x1 < 0 || x1 >= width) x1 = x;
				if(y0 < 0 || y0 >= height) y0 = y;
				if(y1 < 0 || y1 >= height) y1 = y;
				if(z0 < 0 || z0 >= depth) z0 = z;
				if(z1 < 0 || z1 >= depth) z1 = z;
					
								
				running += ((unsigned char*)data)[(z0 * width * height + y0 * width + x0) * pixelSize];
				running += ((unsigned char*)data)[(z0 * width * height + y0 * width + x) * pixelSize];
				running += ((unsigned char*)data)[(z0 * width * height + y0 * width + x1) * pixelSize];
				running += ((unsigned char*)data)[(z0 * width * height + y * width + x0) * pixelSize];
				running += ((unsigned char*)data)[(z0 * width * height + y * width + x) * pixelSize];
				running += ((unsigned char*)data)[(z0 * width * height + y * width + x1) * pixelSize];
				running += ((unsigned char*)data)[(z0 * width * height + y1 * width + x0) * pixelSize];
				running += ((unsigned char*)data)[(z0 * width * height + y1 * width + x) * pixelSize];
				running += ((unsigned char*)data)[(z0 * width * height + y1 * width + x1) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y0 * width + x0) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y0 * width + x) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y0 * width + x1) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y * width + x0) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y * width + x1) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y1 * width + x0) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y1 * width + x) * pixelSize];
				running += ((unsigned char*)data)[(z * width * height + y1 * width + x1) * pixelSize];
				running += ((unsigned char*)data)[(z1 * width * height + y0 * width + x0) * pixelSize];
				running += ((unsigned char*)data)[(z1 * width * height + y0 * width + x) * pixelSize];
				running += ((unsigned char*)data)[(z1 * width * height + y0 * width + x1) * pixelSize];
				running += ((unsigned char*)data)[(z1 * width * height + y * width + x0) * pixelSize];
				running += ((unsigned char*)data)[(z1 * width * height + y * width + x) * pixelSize];
				running += ((unsigned char*)data)[(z1 * width * height + y * width + x1) * pixelSize];
				running += ((unsigned char*)data)[(z1 * width * height + y1 * width + x0) * pixelSize];
				running += ((unsigned char*)data)[(z1 * width * height + y1 * width + x) * pixelSize];
				running += ((unsigned char*)data)[(z1 * width * height + y1 * width + x1) * pixelSize];
				
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 0] = running / 27;
			}
		}
	}
	}
	if(pixelSize == 2)//16 bit monochromeimages
	{
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				int64_t x0 = x+1;
				int64_t x1 = x-1;
				int64_t y0 = y+1;
				int64_t y1 = y-1;
				int64_t z0 = z+1;
				int64_t z1 = z-1;
				
				
				int running = 0; 
				
				if(x0 < 0 || x0 >= width) x0 = x;
				if(x1 < 0 || x1 >= width) x1 = x;
				if(y0 < 0 || y0 >= height) y0 = y;
				if(y1 < 0 || y1 >= height) y1 = y;
				if(z0 < 0 || z0 >= depth) z0 = z;
				if(z1 < 0 || z1 >= depth) z1 = z;
					
								
				running += ((uint16_t*)data)[(z0 * width * height + y0 * width + x0)];
				running += ((uint16_t*)data)[(z0 * width * height + y0 * width + x)];
				running += ((uint16_t*)data)[(z0 * width * height + y0 * width + x1)];
				running += ((uint16_t*)data)[(z0 * width * height + y * width + x0)];
				running += ((uint16_t*)data)[(z0 * width * height + y * width + x)];
				running += ((uint16_t*)data)[(z0 * width * height + y * width + x1)];
				running += ((uint16_t*)data)[(z0 * width * height + y1 * width + x0)];
				running += ((uint16_t*)data)[(z0 * width * height + y1 * width + x) ];
				running += ((uint16_t*)data)[(z0 * width * height + y1 * width + x1)];
				running += ((uint16_t*)data)[(z * width * height + y0 * width + x0)];
				running += ((uint16_t*)data)[(z * width * height + y0 * width + x)];
				running += ((uint16_t*)data)[(z * width * height + y0 * width + x1)];
				running += ((uint16_t*)data)[(z * width * height + y * width + x0)];
				running += ((uint16_t*)data)[(z * width * height + y * width + x)];
				running += ((uint16_t*)data)[(z * width * height + y * width + x1)];
				running += ((uint16_t*)data)[(z * width * height + y1 * width + x0)];
				running += ((uint16_t*)data)[(z * width * height + y1 * width + x)];
				running += ((uint16_t*)data)[(z * width * height + y1 * width + x1)];
				running += ((uint16_t*)data)[(z1 * width * height + y0 * width + x0)];
				running += ((uint16_t*)data)[(z1 * width * height + y0 * width + x)];
				running += ((uint16_t*)data)[(z1 * width * height + y0 * width + x1)];
				running += ((uint16_t*)data)[(z1 * width * height + y * width + x0)];
				running += ((uint16_t*)data)[(z1 * width * height + y * width + x)];
				running += ((uint16_t*)data)[(z1 * width * height + y * width + x1)];
				running += ((uint16_t*)data)[(z1 * width * height + y1 * width + x0) ];
				running += ((uint16_t*)data)[(z1 * width * height + y1 * width + x)];
				running += ((uint16_t*)data)[(z1 * width * height + y1 * width + x1)];
				
				((uint16_t*)data)[(z * width * height + y * width + x)] = running / 27;
			}
		}
	}
	}
}

void Image3D::Median()
{
	if(pixelSize == 1)//8 bit monochrome images
	{
	std::vector<int> vals(27); 
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				int64_t x0 = x+1;
				int64_t x1 = x-1;
				int64_t y0 = y+1;
				int64_t y1 = y-1;
				int64_t z0 = z+1;
				int64_t z1 = z-1;
				
				
				
				if(x0 < 0 || x0 >= width) x0 = x;
				if(x1 < 0 || x1 >= width) x1 = x;
				if(y0 < 0 || y0 >= height) y0 = y;
				if(y1 < 0 || y1 >= height) y1 = y;
				if(z0 < 0 || z0 >= depth) z0 = z;
				if(z1 < 0 || z1 >= depth) z1 = z;
					
								
				vals[0] = ((unsigned char*)data)[(z0 * width * height + y0 * width + x0) * pixelSize];
				vals[1] = ((unsigned char*)data)[(z0 * width * height + y0 * width + x) * pixelSize];
				vals[2] = ((unsigned char*)data)[(z0 * width * height + y0 * width + x1) * pixelSize];
				vals[3] = ((unsigned char*)data)[(z0 * width * height + y * width + x0) * pixelSize];
				vals[4] = ((unsigned char*)data)[(z0 * width * height + y * width + x) * pixelSize];
				vals[5] = ((unsigned char*)data)[(z0 * width * height + y * width + x1) * pixelSize];
				vals[6] = ((unsigned char*)data)[(z0 * width * height + y1 * width + x0) * pixelSize];
				vals[7] = ((unsigned char*)data)[(z0 * width * height + y1 * width + x) * pixelSize];
				vals[8] = ((unsigned char*)data)[(z0 * width * height + y1 * width + x1) * pixelSize];
				vals[9] = ((unsigned char*)data)[(z * width * height + y0 * width + x0) * pixelSize];
				vals[10] = ((unsigned char*)data)[(z * width * height + y0 * width + x) * pixelSize];
				vals[11] = ((unsigned char*)data)[(z * width * height + y0 * width + x1) * pixelSize];
				vals[12] = ((unsigned char*)data)[(z * width * height + y * width + x0) * pixelSize];
				vals[13] = ((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize];
				vals[14] = ((unsigned char*)data)[(z * width * height + y * width + x1) * pixelSize];
				vals[15] = ((unsigned char*)data)[(z * width * height + y1 * width + x0) * pixelSize];
				vals[16] = ((unsigned char*)data)[(z * width * height + y1 * width + x) * pixelSize];
				vals[17] = ((unsigned char*)data)[(z * width * height + y1 * width + x1) * pixelSize];
				vals[18] = ((unsigned char*)data)[(z1 * width * height + y0 * width + x0) * pixelSize];
				vals[19] = ((unsigned char*)data)[(z1 * width * height + y0 * width + x) * pixelSize];
				vals[20] = ((unsigned char*)data)[(z1 * width * height + y0 * width + x1) * pixelSize];
				vals[21] = ((unsigned char*)data)[(z1 * width * height + y * width + x0) * pixelSize];
				vals[22] = ((unsigned char*)data)[(z1 * width * height + y * width + x) * pixelSize];
				vals[23] = ((unsigned char*)data)[(z1 * width * height + y * width + x1) * pixelSize];
				vals[24] = ((unsigned char*)data)[(z1 * width * height + y1 * width + x0) * pixelSize];
				vals[25] = ((unsigned char*)data)[(z1 * width * height + y1 * width + x) * pixelSize];
				vals[26] = ((unsigned char*)data)[(z1 * width * height + y1 * width + x1) * pixelSize];
				
				
				std::nth_element(vals.begin(), vals.begin() + vals.size()/2, vals.end()); 
				
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 0] = vals[vals.size()/2];
			}
		}
	}
	}
	if(pixelSize == 2)//16 bit monochromeimages
	{
	std::vector<int> vals(27); 
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				int64_t x0 = x+1;
				int64_t x1 = x-1;
				int64_t y0 = y+1;
				int64_t y1 = y-1;
				int64_t z0 = z+1;
				int64_t z1 = z-1;
				
				
				
				if(x0 < 0 || x0 >= width) x0 = x;
				if(x1 < 0 || x1 >= width) x1 = x;
				if(y0 < 0 || y0 >= height) y0 = y;
				if(y1 < 0 || y1 >= height) y1 = y;
				if(z0 < 0 || z0 >= depth) z0 = z;
				if(z1 < 0 || z1 >= depth) z1 = z;
					
								
				vals[0] = ((uint16_t*)data)[(z0 * width * height + y0 * width + x0)];
				vals[1] = ((uint16_t*)data)[(z0 * width * height + y0 * width + x) ];
				vals[2] = ((uint16_t*)data)[(z0 * width * height + y0 * width + x1) ];
				vals[3] = ((uint16_t*)data)[(z0 * width * height + y * width + x0) ];
				vals[4] = ((uint16_t*)data)[(z0 * width * height + y * width + x) ];
				vals[5] = ((uint16_t*)data)[(z0 * width * height + y * width + x1) ];
				vals[6] = ((uint16_t*)data)[(z0 * width * height + y1 * width + x0) ];
				vals[7] = ((uint16_t*)data)[(z0 * width * height + y1 * width + x) ];
				vals[8] = ((uint16_t*)data)[(z0 * width * height + y1 * width + x1) ];
				vals[9] = ((uint16_t*)data)[(z * width * height + y0 * width + x0) ];
				vals[10] = ((uint16_t*)data)[(z * width * height + y0 * width + x) ];
				vals[11] = ((uint16_t*)data)[(z * width * height + y0 * width + x1) ];
				vals[12] = ((uint16_t*)data)[(z * width * height + y * width + x0) ];
				vals[13] = ((uint16_t*)data)[(z * width * height + y * width + x) ];
				vals[14] = ((uint16_t*)data)[(z * width * height + y * width + x1) ];
				vals[15] = ((uint16_t*)data)[(z * width * height + y1 * width + x0) ];
				vals[16] = ((uint16_t*)data)[(z * width * height + y1 * width + x) ];
				vals[17] = ((uint16_t*)data)[(z * width * height + y1 * width + x1) ];
				vals[18] = ((uint16_t*)data)[(z1 * width * height + y0 * width + x0) ];
				vals[19] = ((uint16_t*)data)[(z1 * width * height + y0 * width + x) ];
				vals[20] = ((uint16_t*)data)[(z1 * width * height + y0 * width + x1) ];
				vals[21] = ((uint16_t*)data)[(z1 * width * height + y * width + x0) ];
				vals[22] = ((uint16_t*)data)[(z1 * width * height + y * width + x) ];
				vals[23] = ((uint16_t*)data)[(z1 * width * height + y * width + x1) ];
				vals[24] = ((uint16_t*)data)[(z1 * width * height + y1 * width + x0) ];
				vals[25] = ((uint16_t*)data)[(z1 * width * height + y1 * width + x) ];
				vals[26] = ((uint16_t*)data)[(z1 * width * height + y1 * width + x1) ];
				
				
				std::nth_element(vals.begin(), vals.begin() + vals.size()/2, vals.end()); 
				
				((uint16_t*)data)[(z * width * height + y * width + x)] = vals[vals.size()/2];
			}
		}
	}
	}
}

void Image3D::CentralDifference(Image3D& inImg)
{
	if(inImg.pixelSize == 1)//8 bit monochrome images
	{
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				int64_t x0 = x+1;
				int64_t x1 = x-1;
				int64_t y0 = y+1;
				int64_t y1 = y-1;
				int64_t z0 = z+1;
				int64_t z1 = z-1;
				
				unsigned char* indata = (unsigned char*)inImg.Data();
				
				int dx0;
				int dx1;
				int dy0;
				int dy1;
				int dz0;
				int dz1; 
				
				if(x0 < 0 || x0 >= width) x0 = x;
				if(x1 < 0 || x1 >= width) x1 = x;
				if(y0 < 0 || y0 >= height) y0 = y;
				if(y1 < 0 || y1 >= height) y1 = y;
				if(z0 < 0 || z0 >= depth) z0 = z;
				if(z1 < 0 || z1 >= depth) z1 = z;
					
				dx0 = indata[(z * width * height + y * width + x0) * inImg.pixelSize];
				dx1 = indata[(z * width * height + y * width + x1) * inImg.pixelSize];
				dy0 = indata[(z * width * height + y0 * width + x) * inImg.pixelSize];
				dy1 = indata[(z * width * height + y1 * width + x) * inImg.pixelSize];
				dz0 = indata[(z0 * width * height + y * width + x) * inImg.pixelSize];
				dz1 = indata[(z1 * width * height + y * width + x) * inImg.pixelSize];
				
				int gX = ((dx1 - dx0) + 255) / 2;
				int gY = ((dy1 - dy0) + 255) / 2; 
				int gZ = ((dz1 - dz0) + 255) / 2;
				
				
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 0] = gX;
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 1] = gY;
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 2] = gZ;
			}
		}
	}
	}
	if(inImg.pixelSize == 2)//16 bit monochromeimages
	{
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				int64_t x0 = x+1;
				int64_t x1 = x-1;
				int64_t y0 = y+1;
				int64_t y1 = y-1;
				int64_t z0 = z+1;
				int64_t z1 = z-1;
				
				uint16_t* indata = (uint16_t*)inImg.Data();
				
				int dx0;
				int dx1;
				int dy0;
				int dy1;
				int dz0;
				int dz1; 
				
				if(x0 < 0 || x0 >= width) x0 = x;
				if(x1 < 0 || x1 >= width) x1 = x;
				if(y0 < 0 || y0 >= height) y0 = y;
				if(y1 < 0 || y1 >= height) y1 = y;
				if(z0 < 0 || z0 >= depth) z0 = z;
				if(z1 < 0 || z1 >= depth) z1 = z;
					
				dx0 = indata[(z * width * height + y * width + x0)];
				dx1 = indata[(z * width * height + y * width + x1)];
				dy0 = indata[(z * width * height + y0 * width + x)];
				dy1 = indata[(z * width * height + y1 * width + x)];
				dz0 = indata[(z0 * width * height + y * width + x)];
				dz1 = indata[(z1 * width * height + y * width + x)];
				
				int gX = ((dx1 - dx0) + 65535) / 2;
				int gY = ((dy1 - dy0) + 65535) / 2; 
				int gZ = ((dz1 - dz0) + 65535) / 2;
				
				
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 0] = gX * (255.0 / 65535.0);
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 1] = gY * (255.0 / 65535.0);
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 2] = gZ * (255.0 / 65535.0);
			}
		}
	}	
	}
	
}

void Image3D::Sobel(Image3D& inImg)
{
	if(inImg.pixelSize == 1)//8 bit monochrome images
	{
	unsigned char* d = (unsigned char*)inImg.data;
	
	std::vector<int> vals(27, 0); 
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				
				int64_t x0 = x+1;
				int64_t x1 = x-1;
				int64_t y0 = y+1;
				int64_t y1 = y-1;
				int64_t z0 = z+1;
				int64_t z1 = z-1;
				
				
				if(x0 < 0 || x0 >= width) x0 = x;
				if(x1 < 0 || x1 >= width) x1 = x;
				if(y0 < 0 || y0 >= height) y0 = y;
				if(y1 < 0 || y1 >= height) y1 = y;
				if(z0 < 0 || z0 >= depth) z0 = z;
				if(z1 < 0 || z1 >= depth) z1 = z;
					
					
				
					
				//xy plane				z0
				vals[0] = d[(z0 * width * height + y0 * width + x0) * inImg.pixelSize];
				vals[1] = d[(z0 * width * height + y0 * width + x) * inImg.pixelSize];
				vals[2] = d[(z0 * width * height + y0 * width + x1) * inImg.pixelSize];
				vals[3] = d[(z0 * width * height + y * width + x0) * inImg.pixelSize];
				vals[4] = d[(z0 * width * height + y * width + x) * inImg.pixelSize];
				vals[5] = d[(z0 * width * height + y * width + x1) * inImg.pixelSize];
				vals[6] = d[(z0 * width * height + y1 * width + x0) * inImg.pixelSize];
				vals[7] = d[(z0 * width * height + y1 * width + x) * inImg.pixelSize];
				vals[8] = d[(z0 * width * height + y1 * width + x1) * inImg.pixelSize];
				
				//xy plane     z1
				vals[9] = d[(z * width * height + y0 * width + x0) * inImg.pixelSize];
				vals[10] = d[(z * width * height + y0 * width + x) * inImg.pixelSize];
				vals[11] = d[(z * width * height + y0 * width + x1) * inImg.pixelSize];
				vals[12] = d[(z * width * height + y * width + x0) * inImg.pixelSize];
				vals[13] = d[(z * width * height + y * width + x) * inImg.pixelSize];
				vals[14] = d[(z * width * height + y * width + x1) * inImg.pixelSize];
				vals[15] = d[(z * width * height + y1 * width + x0) * inImg.pixelSize];
				vals[16] = d[(z * width * height + y1 * width + x) * inImg.pixelSize];
				vals[17] = d[(z * width * height + y1 * width + x1) * inImg.pixelSize];
				
				//xy plane     z2
				vals[18] = d[(z1 * width * height + y0 * width + x0) * inImg.pixelSize];
				vals[19] = d[(z1 * width * height + y0 * width + x) * inImg.pixelSize];
				vals[20] = d[(z1 * width * height + y0 * width + x1) * inImg.pixelSize];
				vals[21] = d[(z1 * width * height + y * width + x0) * inImg.pixelSize];
				vals[22] = d[(z1 * width * height + y * width + x) * inImg.pixelSize];
				vals[23] = d[(z1 * width * height + y * width + x1) * inImg.pixelSize];
				vals[24] = d[(z1 * width * height + y1 * width + x0) * inImg.pixelSize];
				vals[25] = d[(z1 * width * height + y1 * width + x) * inImg.pixelSize];
				vals[26] = d[(z1 * width * height + y1 * width + x1) * inImg.pixelSize];
				
				
				
				double gradZ =(vals[0] * -1.0 + vals[1] * -2.0 + vals[2] * -1.0 +  
							   vals[3] * -2.0 + vals[4] * -4.0 + vals[5] * -2.0 +  
							   vals[6] * -1.0 + vals[7] * -2.0 + vals[8] * -1.0 + 
							   
							   vals[18] * 1.0 + vals[19] * 2.0 + vals[20] * 1.0 +  
							   vals[21] * 2.0 + vals[22] * 4.0 + vals[23] * 2.0 +  
							   vals[24] * 1.0 + vals[25] * 2.0 + vals[26] * 1.0)/22.0;
							   
							   
				double gradY =(vals[0] * -1.0 + vals[1] * -2.0 + vals[2] * -1.0 + 
							   vals[3] * 0    + vals[4] * 0    + vals[5] * 0 + 
							   vals[6] * 1.0  + vals[7] * 2.0  + vals[8] * 1.0 + 
							   
							   vals[9] * -2.0 + vals[10] * -4.0 + vals[11] * -2.0 + 
							   vals[12] * 0   + vals[13] * 0    + vals[14] * 0 + 
							   vals[15] * 2.0 + vals[16] * 4.0  + vals[17] * 2.0 + 
							   
							   vals[18] * -1.0 + vals[19] * -2.0 + vals[20] * -1.0 + 
							   vals[21] * 0    + vals[22] * 0    + vals[23] * 0 + 
							   vals[24] * 1.0  + vals[25] * 2.0  + vals[26] * 1.0)/22.0;
							   
				double gradX = (vals[0] * -1.0 + vals[1] * 0 + vals[2] * 1.0 + 
							   vals[3] * -2.0  + vals[4] * 0 + vals[5] * 2.0 + 
							   vals[6] * -1.0  + vals[7] * 0 + vals[8] * 1.0 + 
							   
							   vals[9] *  -2.0 + vals[10] * 0 + vals[11] * 2.0 + 
							   vals[12] * -4.0 + vals[13] * 0 + vals[14] * 4.0 + 
							   vals[15] * -2.0 + vals[16] * 0 + vals[17] * 2.0 + 
							   
							   vals[18] * -1.0 + vals[19] * 0 + vals[20] * 1.0 + 
							   vals[21] * -2.0 + vals[22] * 0 + vals[23] * 2.0 + 
							   vals[24] * -1.0 + vals[25] * 0 + vals[26] * 1.0)/22.0;
				
				
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 0] = ((int)gradX + 255) / 2;
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 1] = ((int)gradY + 255) / 2;
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 2] = ((int)gradZ + 255) / 2;
			}
		}
	}
	}
	if(inImg.pixelSize == 2)//16 bit monochromeimages
	{
	uint16_t* d = (uint16_t*)inImg.data;
	
	std::vector<int> vals(27, 0); 
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				
				int64_t x0 = x+1;
				int64_t x1 = x-1;
				int64_t y0 = y+1;
				int64_t y1 = y-1;
				int64_t z0 = z+1;
				int64_t z1 = z-1;
				
				
				if(x0 < 0 || x0 >= width) x0 = x;
				if(x1 < 0 || x1 >= width) x1 = x;
				if(y0 < 0 || y0 >= height) y0 = y;
				if(y1 < 0 || y1 >= height) y1 = y;
				if(z0 < 0 || z0 >= depth) z0 = z;
				if(z1 < 0 || z1 >= depth) z1 = z;
					
					
				
					
				//xy plane				z0
				vals[0] = d[(z0 * width * height + y0 * width + x0)];
				vals[1] = d[(z0 * width * height + y0 * width + x) ];
				vals[2] = d[(z0 * width * height + y0 * width + x1) ];
				vals[3] = d[(z0 * width * height + y * width + x0) ];
				vals[4] = d[(z0 * width * height + y * width + x) ];
				vals[5] = d[(z0 * width * height + y * width + x1) ];
				vals[6] = d[(z0 * width * height + y1 * width + x0) ];
				vals[7] = d[(z0 * width * height + y1 * width + x) ];
				vals[8] = d[(z0 * width * height + y1 * width + x1) ];
				
				//xy plane     z1
				vals[9] = d[(z * width * height + y0 * width + x0) ];
				vals[10] = d[(z * width * height + y0 * width + x) ];
				vals[11] = d[(z * width * height + y0 * width + x1) ];
				vals[12] = d[(z * width * height + y * width + x0) ];
				vals[13] = d[(z * width * height + y * width + x) ];
				vals[14] = d[(z * width * height + y * width + x1) ];
				vals[15] = d[(z * width * height + y1 * width + x0) ];
				vals[16] = d[(z * width * height + y1 * width + x) ];
				vals[17] = d[(z * width * height + y1 * width + x1) ];
				
				//xy plane     z2
				vals[18] = d[(z1 * width * height + y0 * width + x0) ];
				vals[19] = d[(z1 * width * height + y0 * width + x) ];
				vals[20] = d[(z1 * width * height + y0 * width + x1) ];
				vals[21] = d[(z1 * width * height + y * width + x0) ];
				vals[22] = d[(z1 * width * height + y * width + x) ];
				vals[23] = d[(z1 * width * height + y * width + x1) ];
				vals[24] = d[(z1 * width * height + y1 * width + x0) ];
				vals[25] = d[(z1 * width * height + y1 * width + x) ];
				vals[26] = d[(z1 * width * height + y1 * width + x1) ];
				
				
				
				double gradZ =(vals[0] * -1.0 + vals[1] * -2.0 + vals[2] * -1.0 +  
							   vals[3] * -2.0 + vals[4] * -4.0 + vals[5] * -2.0 +  
							   vals[6] * -1.0 + vals[7] * -2.0 + vals[8] * -1.0 + 
							   
							   vals[18] * 1.0 + vals[19] * 2.0 + vals[20] * 1.0 +  
							   vals[21] * 2.0 + vals[22] * 4.0 + vals[23] * 2.0 +  
							   vals[24] * 1.0 + vals[25] * 2.0 + vals[26] * 1.0)/22.0;
							   
							   
				double gradY =(vals[0] * -1.0 + vals[1] * -2.0 + vals[2] * -1.0 + 
							   vals[3] * 0    + vals[4] * 0    + vals[5] * 0 + 
							   vals[6] * 1.0  + vals[7] * 2.0  + vals[8] * 1.0 + 
							   
							   vals[9] * -2.0 + vals[10] * -4.0 + vals[11] * -2.0 + 
							   vals[12] * 0   + vals[13] * 0    + vals[14] * 0 + 
							   vals[15] * 2.0 + vals[16] * 4.0  + vals[17] * 2.0 + 
							   
							   vals[18] * -1.0 + vals[19] * -2.0 + vals[20] * -1.0 + 
							   vals[21] * 0    + vals[22] * 0    + vals[23] * 0 + 
							   vals[24] * 1.0  + vals[25] * 2.0  + vals[26] * 1.0)/22.0;
							   
				double gradX = (vals[0] * -1.0 + vals[1] * 0 + vals[2] * 1.0 + 
							   vals[3] * -2.0  + vals[4] * 0 + vals[5] * 2.0 + 
							   vals[6] * -1.0  + vals[7] * 0 + vals[8] * 1.0 + 
							   
							   vals[9] *  -2.0 + vals[10] * 0 + vals[11] * 2.0 + 
							   vals[12] * -4.0 + vals[13] * 0 + vals[14] * 4.0 + 
							   vals[15] * -2.0 + vals[16] * 0 + vals[17] * 2.0 + 
							   
							   vals[18] * -1.0 + vals[19] * 0 + vals[20] * 1.0 + 
							   vals[21] * -2.0 + vals[22] * 0 + vals[23] * 2.0 + 
							   vals[24] * -1.0 + vals[25] * 0 + vals[26] * 1.0)/22.0;
				
				
				double gX = (gradX + 65535.0) / 2.0;
				double gY = (gradY + 65535.0) / 2.0;
				double gZ = (gradZ + 65535.0) / 2.0;
				
				
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 0] = gX * (255.0 / 65535.0);
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 1] = gY * (255.0 / 65535.0) ;
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 2] = gZ * (255.0 / 65535.0);
			}
		}
	}
	}
}

void Image3D::Sobel2(Image3D& inImg)
{
	if(inImg.pixelSize == 1)//8 bit monochrome images
	{
	unsigned char* d = (unsigned char*)inImg.data;
	
	std::vector<int> vals(27, 0); 
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				
				int64_t x0 = x+1;
				int64_t x1 = x-1;
				int64_t y0 = y+1;
				int64_t y1 = y-1;
				int64_t z0 = z+1;
				int64_t z1 = z-1;
				
				
				if(x0 < 0 || x0 >= width) x0 = x;
				if(x1 < 0 || x1 >= width) x1 = x;
				if(y0 < 0 || y0 >= height) y0 = y;
				if(y1 < 0 || y1 >= height) y1 = y;
				if(z0 < 0 || z0 >= depth) z0 = z;
				if(z1 < 0 || z1 >= depth) z1 = z;
					
					
				
					
				//xy plane				z0
				vals[0] = d[(z0 * width * height + y0 * width + x0) * inImg.pixelSize];
				vals[1] = d[(z0 * width * height + y0 * width + x) * inImg.pixelSize];
				vals[2] = d[(z0 * width * height + y0 * width + x1) * inImg.pixelSize];
				vals[3] = d[(z0 * width * height + y * width + x0) * inImg.pixelSize];
				vals[4] = d[(z0 * width * height + y * width + x) * inImg.pixelSize];
				vals[5] = d[(z0 * width * height + y * width + x1) * inImg.pixelSize];
				vals[6] = d[(z0 * width * height + y1 * width + x0) * inImg.pixelSize];
				vals[7] = d[(z0 * width * height + y1 * width + x) * inImg.pixelSize];
				vals[8] = d[(z0 * width * height + y1 * width + x1) * inImg.pixelSize];
				
				//xy plane     z1
				vals[9] = d[(z * width * height + y0 * width + x0) * inImg.pixelSize];
				vals[10] = d[(z * width * height + y0 * width + x) * inImg.pixelSize];
				vals[11] = d[(z * width * height + y0 * width + x1) * inImg.pixelSize];
				vals[12] = d[(z * width * height + y * width + x0) * inImg.pixelSize];
				vals[13] = d[(z * width * height + y * width + x) * inImg.pixelSize];
				vals[14] = d[(z * width * height + y * width + x1) * inImg.pixelSize];
				vals[15] = d[(z * width * height + y1 * width + x0) * inImg.pixelSize];
				vals[16] = d[(z * width * height + y1 * width + x) * inImg.pixelSize];
				vals[17] = d[(z * width * height + y1 * width + x1) * inImg.pixelSize];
				
				//xy plane     z2
				vals[18] = d[(z1 * width * height + y0 * width + x0) * inImg.pixelSize];
				vals[19] = d[(z1 * width * height + y0 * width + x) * inImg.pixelSize];
				vals[20] = d[(z1 * width * height + y0 * width + x1) * inImg.pixelSize];
				vals[21] = d[(z1 * width * height + y * width + x0) * inImg.pixelSize];
				vals[22] = d[(z1 * width * height + y * width + x) * inImg.pixelSize];
				vals[23] = d[(z1 * width * height + y * width + x1) * inImg.pixelSize];
				vals[24] = d[(z1 * width * height + y1 * width + x0) * inImg.pixelSize];
				vals[25] = d[(z1 * width * height + y1 * width + x) * inImg.pixelSize];
				vals[26] = d[(z1 * width * height + y1 * width + x1) * inImg.pixelSize];
				
				
				
				double gradZ =(vals[0] * -1.0 + vals[1] * -3.0 + vals[2] * -1.0 +  
							   vals[3] * -3.0 + vals[4] * -6.0 + vals[5] * -3.0 +  
							   vals[6] * -1.0 + vals[7] * -3.0 + vals[8] * -1.0 + 
							   
							   vals[18] * 1.0 + vals[19] * 3.0 + vals[20] * 1.0 +  
							   vals[21] * 3.0 + vals[22] * 6.0 + vals[23] * 3.0 +  
							   vals[24] * 1.0 + vals[25] * 3.0 + vals[26] * 1.0)/22.0;
							   
							   
				double gradY =(vals[0] * -1.0 + vals[1] * -3.0 + vals[2] * -1.0 + 
							   vals[3] * 0    + vals[4] * 0    + vals[5] * 0 + 
							   vals[6] * 1.0  + vals[7] * 3.0  + vals[8] * 1.0 + 
							   
							   vals[9] * -3.0 + vals[10] * -6.0 + vals[11] * -3.0 + 
							   vals[12] * 0   + vals[13] * 0    + vals[14] * 0 + 
							   vals[15] * 3.0 + vals[16] * 6.0  + vals[17] * 3.0 + 
							   
							   vals[18] * -1.0 + vals[19] * -3.0 + vals[20] * -1.0 + 
							   vals[21] * 0    + vals[22] * 0    + vals[23] * 0 + 
							   vals[24] * 1.0  + vals[25] * 3.0  + vals[26] * 1.0)/22.0;
							   
				double gradX = (vals[0] * -1.0 + vals[1] * 0 + vals[2] * 1.0 + 
							   vals[3] * -3.0  + vals[4] * 0 + vals[5] * 3.0 + 
							   vals[6] * -1.0  + vals[7] * 0 + vals[8] * 1.0 + 
							   
							   vals[9] *  -3.0 + vals[10] * 0 + vals[11] * 3.0 + 
							   vals[12] * -6.0 + vals[13] * 0 + vals[14] * 6.0 + 
							   vals[15] * -3.0 + vals[16] * 0 + vals[17] * 3.0 + 
							   
							   vals[18] * -1.0 + vals[19] * 0 + vals[20] * 1.0 + 
							   vals[21] * -3.0 + vals[22] * 0 + vals[23] * 3.0 + 
							   vals[24] * -1.0 + vals[25] * 0 + vals[26] * 1.0)/22.0;
				
				
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 0] = ((int)gradX + 255) / 2;
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 1] = ((int)gradY + 255) / 2;
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 2] = ((int)gradZ + 255) / 2;
			}
		}
	}
	}
	if(inImg.pixelSize == 2)//16 bit monochromeimages
	{
	uint16_t* d = (uint16_t*)inImg.data;
	
	std::vector<int> vals(27, 0); 
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				
				int64_t x0 = x+1;
				int64_t x1 = x-1;
				int64_t y0 = y+1;
				int64_t y1 = y-1;
				int64_t z0 = z+1;
				int64_t z1 = z-1;
				
				
				if(x0 < 0 || x0 >= width) x0 = x;
				if(x1 < 0 || x1 >= width) x1 = x;
				if(y0 < 0 || y0 >= height) y0 = y;
				if(y1 < 0 || y1 >= height) y1 = y;
				if(z0 < 0 || z0 >= depth) z0 = z;
				if(z1 < 0 || z1 >= depth) z1 = z;
					
					
				
					
				//xy plane				z0
				vals[0] = d[(z0 * width * height + y0 * width + x0)];
				vals[1] = d[(z0 * width * height + y0 * width + x) ];
				vals[2] = d[(z0 * width * height + y0 * width + x1) ];
				vals[3] = d[(z0 * width * height + y * width + x0) ];
				vals[4] = d[(z0 * width * height + y * width + x) ];
				vals[5] = d[(z0 * width * height + y * width + x1) ];
				vals[6] = d[(z0 * width * height + y1 * width + x0) ];
				vals[7] = d[(z0 * width * height + y1 * width + x) ];
				vals[8] = d[(z0 * width * height + y1 * width + x1) ];
				
				//xy plane     z1
				vals[9] = d[(z * width * height + y0 * width + x0) ];
				vals[10] = d[(z * width * height + y0 * width + x) ];
				vals[11] = d[(z * width * height + y0 * width + x1) ];
				vals[12] = d[(z * width * height + y * width + x0) ];
				vals[13] = d[(z * width * height + y * width + x) ];
				vals[14] = d[(z * width * height + y * width + x1) ];
				vals[15] = d[(z * width * height + y1 * width + x0) ];
				vals[16] = d[(z * width * height + y1 * width + x) ];
				vals[17] = d[(z * width * height + y1 * width + x1) ];
				
				//xy plane     z2
				vals[18] = d[(z1 * width * height + y0 * width + x0) ];
				vals[19] = d[(z1 * width * height + y0 * width + x) ];
				vals[20] = d[(z1 * width * height + y0 * width + x1) ];
				vals[21] = d[(z1 * width * height + y * width + x0) ];
				vals[22] = d[(z1 * width * height + y * width + x) ];
				vals[23] = d[(z1 * width * height + y * width + x1) ];
				vals[24] = d[(z1 * width * height + y1 * width + x0) ];
				vals[25] = d[(z1 * width * height + y1 * width + x) ];
				vals[26] = d[(z1 * width * height + y1 * width + x1) ];
				
				
				
				double gradZ =(vals[0] * -1.0 + vals[1] * -3.0 + vals[2] * -1.0 +  
							   vals[3] * -3.0 + vals[4] * -6.0 + vals[5] * -3.0 +  
							   vals[6] * -1.0 + vals[7] * -3.0 + vals[8] * -1.0 + 
							   
							   vals[18] * 1.0 + vals[19] * 3.0 + vals[20] * 1.0 +  
							   vals[21] * 3.0 + vals[22] * 6.0 + vals[23] * 3.0 +  
							   vals[24] * 1.0 + vals[25] * 3.0 + vals[26] * 1.0)/22.0;
							   
							   
				double gradY =(vals[0] * -1.0 + vals[1] * -3.0 + vals[2] * -1.0 + 
							   vals[3] * 0    + vals[4] * 0    + vals[5] * 0 + 
							   vals[6] * 1.0  + vals[7] * 3.0  + vals[8] * 1.0 + 
							   
							   vals[9] * -3.0 + vals[10] * -6.0 + vals[11] * -3.0 + 
							   vals[12] * 0   + vals[13] * 0    + vals[14] * 0 + 
							   vals[15] * 3.0 + vals[16] * 6.0  + vals[17] * 3.0 + 
							   
							   vals[18] * -1.0 + vals[19] * -3.0 + vals[20] * -1.0 + 
							   vals[21] * 0    + vals[22] * 0    + vals[23] * 0 + 
							   vals[24] * 1.0  + vals[25] * 3.0  + vals[26] * 1.0)/22.0;
							   
				double gradX = (vals[0] * -1.0 + vals[1] * 0 + vals[2] * 1.0 + 
							   vals[3] * -3.0  + vals[4] * 0 + vals[5] * 3.0 + 
							   vals[6] * -1.0  + vals[7] * 0 + vals[8] * 1.0 + 
							   
							   vals[9] *  -3.0 + vals[10] * 0 + vals[11] * 3.0 + 
							   vals[12] * -6.0 + vals[13] * 0 + vals[14] * 6.0 + 
							   vals[15] * -3.0 + vals[16] * 0 + vals[17] * 3.0 + 
							   
							   vals[18] * -1.0 + vals[19] * 0 + vals[20] * 1.0 + 
							   vals[21] * -3.0 + vals[22] * 0 + vals[23] * 3.0 + 
							   vals[24] * -1.0 + vals[25] * 0 + vals[26] * 1.0)/22.0;
				
				
				double gX = (gradX + 65535.0) / 2.0;
				double gY = (gradY + 65535.0) / 2.0;
				double gZ = (gradZ + 65535.0) / 2.0;
				
				
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 0] = gX * (255.0 / 65535.0);
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 1] = gY * (255.0 / 65535.0) ;
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 2] = gZ * (255.0 / 65535.0);
			}
		}
	}
	}
}

void Image3D::Normalize()
{
	if(pixelSize == 1)//8 bit monochrome images
	{
	unsigned char minD = 255;
	unsigned char maxD = 0; 
	
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				
				unsigned char* indata = (unsigned char*)Data();
				
				unsigned char d = indata[(z * width * height + y * width + x) * pixelSize];
				
				if(d < minD )
					minD = d;
				
				if(d > maxD)
					maxD = d; 
			
			}
		}
	}
	
	double scaleFactor = 255.0 / (double)(maxD - minD); 
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				unsigned char* indata = (unsigned char*)Data();
				
				unsigned char d = indata[(z * width * height + y * width + x) * pixelSize];
				
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 0] = (d - minD) * scaleFactor;
			}
		}
	}
	}
	if(pixelSize == 2)//16 bit monochromeimages
	{
	uint16_t minD = 65535;
	uint16_t maxD = 0; 
	
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				
				uint16_t* indata = (uint16_t*)Data();
				
				uint16_t d = indata[(z * width * height + y * width + x)];
				
				if(d < minD )
					minD = d;
				
				if(d > maxD)
					maxD = d; 
			
			}
		}
	}
	
	double scaleFactor = 65535.0 / (double)(maxD - minD); 
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				uint16_t* indata = (uint16_t*)Data();
				
				uint16_t d = indata[(z * width * height + y * width + x)];
				
				((uint16_t*)data)[(z * width * height + y * width + x)] = (d - minD) * scaleFactor;
			}
		}
	}
	}
}

void Image3D::Histogram(std::vector<float>* histogram)
{
	if(pixelSize == 1)//8 bit monochrome images
	{
	int bins = 256; 
	histogram->resize(bins, 0);
	
	float maxBinCount = 0;
		
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				unsigned char r = ((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 0];
				(*histogram)[r] += 1.0f;
				if((*histogram)[r] > maxBinCount && r != 0) 
					maxBinCount = (*histogram)[r]; 
			}
		}
	}
	
	for(int i = 0; i < bins; i++)
	{
		(*histogram)[i] = (*histogram)[i] / maxBinCount;
	}
	}
	if(pixelSize == 2)//16 bit monochrome images
	{
	int bins = 65536; 
	histogram->resize(bins, 0);
	
	float maxBinCount = 0;
		
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				uint16_t r = ((uint16_t*)data)[(z * width * height + y * width + x)];
				(*histogram)[r] += 1.0f;
				if((*histogram)[r] > maxBinCount && r != 0) 
					maxBinCount = (*histogram)[r]; 
			}
		}
	}
	
	for(int i = 0; i < bins; i++)
	{
		(*histogram)[i] = (*histogram)[i] / maxBinCount;
	}
	}
}

void Image3D::BrightnessContrastThreshold(double brightness, double contrast, double threshold)
{
	if(pixelSize == 1)//8 bit monochrome images
	{
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				
				unsigned char v = ((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 0];
				
				double vf = (double)v / 255.0;
				
				vf = vf * contrast + brightness; 
				vf = vf > threshold ? vf : 0.0;
				if(vf < 0.0) vf = 0.0;
				if(vf > 1.0) vf = 1.0; 
				
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 0] = vf * 255;
				
			}
		}
	}
	}
	else if(pixelSize == 2)//16 bit monochrome images
	{
	for(uint64_t z = 0; z < depth; z++)
	{
		for(uint64_t y = 0; y < height; y++)
		{
			for(uint64_t x = 0; x < width; x++)
			{
				
				uint16_t v = ((uint16_t*)data)[(z * width * height + y * width + x)];
				
				double vf = (double)v / 65535.0;
				
				vf = vf * contrast + brightness; 
				vf = vf > threshold ? vf : 0.0;
				if(vf < 0.0) vf = 0.0;
				if(vf > 1.0) vf = 1.0; 
				
				((uint16_t*)data)[(z * width * height + y * width + x)] = vf * 65535;
				
			}
		}
	}	
	}
}