#pragma once


#include "Common.hpp"

#include "Renderer/CameraObject.hpp"
#include "Renderer/TextureVolumeObject.hpp"
#include "Renderer/RayVolumeObject.hpp"
#include "Renderer/Texture3D.hpp"
#include "Renderer/Texture1D.hpp"
#include "Renderer/CameraControl.hpp"
#include "Renderer/AxisObject.hpp"


class RenderViewport: public QOpenGLWidget
{
	Q_OBJECT
	protected:
		int windowWidth;
		int windowHeight; 
	
		void initializeGL();
		void paintGL();
		void resizeGL(int w, int h);
		
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void wheelEvent(QWheelEvent *event);
		void keyPressEvent(QKeyEvent *event);
		void keyReleaseEvent(QKeyEvent *event);
		
	public:
		enum RENDER_TYPE{SLICE_RENDER, RAY_RENDER}; 
		RENDER_TYPE renderType; 
		CameraObject* cameraObject; 
		CameraControl* cameraControl; 
		TextureVolumeObject* textureVolumeObject; 
		RayVolumeObject* rayVolumeObject; 
		Texture3D* textureVolume; 
		Texture3D* textureGradient; 
		Texture1D* textureLUT; 
		AxisObject* axisObject;
		
		RenderViewport();
		
	public slots:
		void EnableDisableAxis(bool en);
		void ImportDicomFile(QString fileName);
		void ImportDicomFileSequence(QStringList fileNames);
		void ImportTIFFFileSequence(QStringList fileNames);
		void LoadLUT(float* buffer, int sizeLUT);
		void ChooseRenderer(RENDER_TYPE rt);
};