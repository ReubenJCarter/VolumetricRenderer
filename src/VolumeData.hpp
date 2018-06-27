#pragma once 


#include "Image3D.hpp"
#include "Renderer/Texture3D.hpp"


class VolumeData
{
	public:
		Image3D intensityImage;
		Image3D gradientImage;
		Texture3D textureVolume; 
		Texture3D textureGradient; 
		std::vector<float> textureVolumeHistogram;
		
		VolumeData(); 
		bool BuildFromImage3D();
		void ImportDicomFile(QString fileName);
		void ImportDicomFileSequence(QStringList fileNames);
		void ImportTIFFFile(QString fileName);
		void ImportImageFile(QString fileName);
		void ImportNRRDFile(QString fileName);
		void ImportTIFFFileSequence(QStringList fileNames);
		void ImportImageFileSequence(QStringList fileNames);
};