#include "VolumeData.hpp"


#include "Util.hpp"

#include "IO/Image3DFromDicomFile.hpp"
#include "IO/Image3DFromDevilFile.hpp"
#include "IO/Image3DFromNRRDFile.hpp"


VolumeData::VolumeData()
{
	
}

void VolumeData::ImportDicomFileSequence(QStringList fileNames)
{
	std::vector<std::string> files;
	for(int i = 0; i < fileNames.size(); i++)
		files.push_back(fileNames.at(i).toStdString());
	bool loadGood = Image3DFromDicomFileSequence(&intensityImage, files);
	if(!loadGood)
		return; 
	textureVolume.Allocate(intensityImage.Width(), intensityImage.Height(), intensityImage.Depth());
	textureVolume.LoadData(intensityImage.Data());
}

void VolumeData::ImportImageFile(QString fileName)
{
	bool loadGood = Image3DFromDevilFile(&intensityImage, fileName.toStdString());
	if(!loadGood)
		return;
	
	loadGood = BuildFromImage3D();
	
	if(!loadGood)
		return;
	
	
}

void VolumeData::ImportNRRDFile(QString fileName)
{
	bool loadGood = Image3DFromNRRDFile(&intensityImage, fileName.toStdString());
	if(!loadGood)
		return;
	
	loadGood = BuildFromImage3D();
	
	if(!loadGood)
		return;
	
	
	
	
}

void VolumeData::ImportImageFileSequence(QStringList fileNames)
{
	std::vector<std::string> files;
	for(int i = 0; i < fileNames.size(); i++)
		files.push_back(fileNames.at(i).toStdString());
	
	bool loadGood = Image3DFromDevilFileSequence(&intensityImage, files);
	if(!loadGood)
		return;
	
	loadGood = BuildFromImage3D();
	
	if(!loadGood)
		return;
	
	
}

bool VolumeData::BuildFromImage3D()
{
	if(intensityImage.Width()  == 0 || intensityImage.Height()  == 0 || intensityImage.Depth()  == 0)
	{
		std::cout << "VolumeData: Could not load image 3d into viewport" << std::endl; 
		return false;
	}
	
	std::cout << "VolumeData: Pre Processing intensity image" << std::endl; 

	//intensityImage.Normalize();
	//intensityImage.Median2D();
	
	std::cout << "VolumeData: Building intensity texture" << std::endl; 
	textureVolume.Allocate(intensityImage.Width(), intensityImage.Height(), intensityImage.Depth(), false, 1, 2);
	textureVolume.LoadData(intensityImage.Data());
	
	std::cout << "VolumeData: Building histogram" << std::endl; 
	intensityImage.Histogram(&textureVolumeHistogram); 
	
	std::cout << "VolumeData: Building gradient image" << std::endl; 
	gradientImage.Allocate(intensityImage.Width(), intensityImage.Height(), intensityImage.Depth(), 3);
	
	gradientImage.Sobel(intensityImage);
	
	std::cout << "VolumeData: Building gradient texture" << std::endl; 
	textureGradient.Allocate(gradientImage.Width(), gradientImage.Height(), gradientImage.Depth(), false, 3);
	textureGradient.LoadData(gradientImage.Data());
	
	
	return true; 
}

void VolumeData::ApplyBCTSettings(double b, double c, double t)
{
	intensityImage.BrightnessContrastThreshold(b, c, t);
	
	
	
	std::cout << "VolumeData: Building intensity texture" << std::endl; 
	textureVolume.Allocate(intensityImage.Width(), intensityImage.Height(), intensityImage.Depth(), false, 1, 2);
	textureVolume.LoadData(intensityImage.Data());
	
	std::cout << "VolumeData: Building histogram" << std::endl; 
	intensityImage.Histogram(&textureVolumeHistogram); 
	
	std::cout << "VolumeData: Building gradient image" << std::endl; 
	gradientImage.Allocate(intensityImage.Width(), intensityImage.Height(), intensityImage.Depth(), 3);
	
	gradientImage.Sobel(intensityImage);
	
	std::cout << "VolumeData: Building gradient texture" << std::endl; 
	textureGradient.Allocate(gradientImage.Width(), gradientImage.Height(), gradientImage.Depth(), false, 3);
	textureGradient.LoadData(gradientImage.Data());
}