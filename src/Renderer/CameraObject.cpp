#include "CameraObject.hpp"


CameraObject::CameraObject()
{
	ortho = false; 
	nearClipping = 0.1f;
	farClipping = 10000.0f;
	fovV = 60.0f; 
}

glm::mat4 CameraObject::GetProjectionMatrix(float windowWidth, float windowHeight)
{
	float aspect = windowWidth / windowHeight; 
	
	float degtorad = 0.01745329251;
	
	glm::mat4 projMat; 
	if(!ortho) 
		projMat = glm::perspectiveLH(degtorad * fovV, aspect, nearClipping, farClipping);
	else//have no idea if this is right yet ... TODO
	{
		float asp = windowWidth / windowHeight; 
		projMat = glm::ortho(-asp/2, asp/2, -0.5f, 0.5f, -10000.0f, 10000.0f);
	}
	return projMat; 
}

glm::mat4 CameraObject::GetViewMatrix()
{
	return glm::inverse(GetModelMatrix());
}

float CameraObject::GetFovV()
{
	return fovV; 
}
