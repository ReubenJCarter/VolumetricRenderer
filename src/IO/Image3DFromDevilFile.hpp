#pragma once

#include "../Common.hpp"
#include "../Image3D.hpp"

bool Image3DFromDevilFileSequence(Image3D* image, std::vector<std::string> fileNames);
bool Image3DFromDevilFile(Image3D* image, std::string fileName);