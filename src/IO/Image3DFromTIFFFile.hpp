#pragma once

#include "../Common.hpp"
#include "../Image3D.hpp"

bool Image3DFromTIFFFileSequence(Image3D* image, std::vector<std::string> fileNames);
bool Image3DFromTIFFFile(Image3D* image, std::string fileName);