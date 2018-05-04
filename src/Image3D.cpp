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

void Image3D::Smooth()
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
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 1] = running / 27;
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 2] = running / 27;
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 3] = running / 27;
			}
		}
	}
}

void Image3D::Median()
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
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 1] = vals[vals.size()/2];
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 2] = vals[vals.size()/2];
				((unsigned char*)data)[(z * width * height + y * width + x) * pixelSize + 3] = vals[vals.size()/2];
			}
		}
	}
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