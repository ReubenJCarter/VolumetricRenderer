#pragma once


#include "Common.hpp"

#include "CameraObject.hpp"


class CameraControl2D: public QObject 
{
	Q_OBJECT
	
	protected:
		bool lmbState;
		bool rmbState; 
		bool mmbState; 
		bool shiftState;
		bool firstMouseMove; 
		int mousePrevX;
		int mousePrevY;
		float zoomSpeed; 
		float zoom; 
		float moveSpeed; 
		float posX;
		float posY;
		float posZ;
		float posXPrev;
		float posYPrev;
		float posZPrev;
		CameraObject* cameraObject; 
		
	signals:
		void CameraUpdated();
		
	public:
		bool enabled; 
	
		CameraControl2D(CameraObject* camObj);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void wheelEvent(QWheelEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void keyPressEvent(QKeyEvent *event);
		void keyReleaseEvent(QKeyEvent *event);
		void UpdateCamera(); 
};