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

void Image3D::Gradient(Image3D& inImg)
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
				
				char* indata = (char*)inImg.Data();
				
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
					
				dx0 = indata[(z * depth * height + y * height + x0) * inImg.pixelSize];
				dx1 = indata[(z * depth * height + y * height + x1) * inImg.pixelSize];
				dy0 = indata[(z * depth * height + y0 * height + x) * inImg.pixelSize];
				dy1 = indata[(z * depth * height + y1 * height + x) * inImg.pixelSize];
				dz0 = indata[(z0 * depth * height + y * height + x) * inImg.pixelSize];
				dz1 = indata[(z1 * depth * height + y * height + x) * inImg.pixelSize];
				
				int gX = ((dx1 - dx0) + 255) / 2;
				int gY = ((dy1 - dy0) + 255) / 2; 
				int gZ = ((dz1 - dz0) + 255) / 2;
				
				((char*)data)[(z * depth * height + y * height + x) * 3 + 0] = gX;
				((char*)data)[(z * depth * height + y * height + x) * 3 + 1] = gY;
				((char*)data)[(z * depth * height + y * height + x) * 3 + 2] = gZ;
			}
		}
	}
}