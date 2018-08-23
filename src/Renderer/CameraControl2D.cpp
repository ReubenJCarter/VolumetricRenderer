#include "CameraControl2D.hpp"


CameraControl2D::CameraControl2D(CameraObject* camObj)
{
	cameraObject = camObj;
	zoom = 1;
	zoomSpeed = 0.06;
	posX = 0;
	posY = 0;
	posZ = -1;
	moveSpeed = 0.005;
	firstMouseMove = true; 
	enabled = true; 
}


void CameraControl2D::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton) 
	{
		lmbState = true; 
		mousePrevX = event->globalX();
		mousePrevY = event->globalY();
	}
	if(event->button() == Qt::RightButton) rmbState = true; 
	if(event->button() == Qt::MiddleButton)
	{
		mmbState = true; 
		mousePrevX = event->globalX();
		mousePrevY = event->globalY();
		posXPrev = posX;
		posYPrev = posY;
		posZPrev = posZ;
	}
	
	if(event->button() == Qt::LeftButton || 
	   event->button() == Qt::RightButton || 
	   event->button() == Qt::MiddleButton)
		UpdateCamera();
}

void CameraControl2D::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{ 
		lmbState = false; 
	}
	if(event->button() == Qt::RightButton) rmbState = false;
	if(event->button() == Qt::MiddleButton) mmbState = false; 	
	
	if(event->button() == Qt::LeftButton || 
	   event->button() == Qt::RightButton || 
	   event->button() == Qt::MiddleButton)
		UpdateCamera();
}

void CameraControl2D::wheelEvent(QWheelEvent *event)
{
	QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) 
	{
        zoom += zoomSpeed * numPixels.y();
		if(zoom < 0.1) zoom = 0.1; 
		UpdateCamera();
		event->accept();
    } 
	else if (!numDegrees.isNull()) 
	{
        QPoint numSteps = numDegrees / 15;
        zoom += zoom * zoomSpeed * numSteps.y();
		if(zoom < 0.1) zoom = 0.1; 
		UpdateCamera();
		event->accept();
    }
	else
	{
		event->ignore(); 
	}
}

void CameraControl2D::mouseMoveEvent(QMouseEvent *event)
{
	if(lmbState)
	{		
		UpdateCamera(); 
	}
	else if(mmbState)
	{
		float degtorad = 0.0174532925;
		
		float deltaX = -moveSpeed * (event->globalX() - mousePrevX) * zoom;
		float deltaY = moveSpeed * (event->globalY() - mousePrevY) * zoom;
			
		glm::vec4 deltaVec = glm::vec4(deltaX, deltaY, 0, 0);
		
		posX = posXPrev + deltaVec.x;
		posY = posYPrev + deltaVec.y;
		posZ = posZPrev + deltaVec.z;
		
		UpdateCamera(); 
	}
}

void CameraControl2D::keyPressEvent(QKeyEvent *event)
{
	
	if(event->key() == Qt::Key_Shift) shiftState = true; 
	
	if(event->key() == Qt::Key_F)
	{
		std::cout << "F!!!" << std::endl; 
		posX = 0;
		posY = 0;
		posZ = 0;
		UpdateCamera(); 
	}
}

void CameraControl2D::keyReleaseEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Shift) shiftState = false; 
}

void CameraControl2D::UpdateCamera()
{
	if(enabled)
	{
		cameraObject->SetRotation(glm::vec3(0, 0, 0));
		
		float degtorad = 0.0174532925;
		float finalPosX = posX + zoom * -1;
		float finalPosY = posY + zoom * 0;
		float finalPosZ = posZ + zoom * -1;
		cameraObject->SetPosition(glm::vec3(finalPosX, finalPosY, finalPosZ));
		
		std::cout << "Camera2d Updated:rot "
				  << " zoom " << zoom 
				  << " pos " << finalPosX << " " << finalPosY << " " << finalPosZ
				  << std::endl; 
		emit CameraUpdated();
	}
}