#pragma once


#include "Common.hpp"

#include "Image3D.hpp"
#include "Renderer/CameraObject.hpp"
#include "Renderer/TextureVolumeObject.hpp"
#include "Renderer/RayVolumeObject.hpp"
#include "Renderer/PhotonVolumeObject.hpp"
#include "Renderer/Texture3D.hpp"
#include "Renderer/TextureCube.hpp"
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
		enum RENDER_TYPE{SLICE_RENDER, RAY_RENDER, PHOTON_RENDER}; 
		RENDER_TYPE renderType; 
		CameraObject* cameraObject; 
		CameraControl* cameraControl; 
		TextureVolumeObject* textureVolumeObject; 
		RayVolumeObject* rayVolumeObject; 
		PhotonVolumeObject* photonVolumeObject;
		Texture3D* textureVolume; 
		Texture3D* textureGradient; 
		TextureCube* textureEnvMap; 
		Texture1D* textureLUT; 
		AxisObject* axisObject;
		std::vector<float> textureVolumeHistogram;
		
		RenderViewport();
		
	public slots:
		void EnableDisableAxis(bool en);
		void ImportDicomFile(QString fileName);
		void ImportDicomFileSequence(QStringList fileNames);
		void ImportTIFFFile(QString fileName, std::vector<float>* histogram=NULL);
		void ImportTIFFFileSequence(QStringList fileNames, std::vector<float>* histogram=NULL);
		bool LoadFromImage3D(Image3D& image3D);
		void LoadLUT(float* buffer, int sizeLUT);
		void LoadDefaultEnvMap();
		void ChooseRenderer(RENDER_TYPE rt);
		void SetGradientThreshold(float threshold);
		void SetBackFaceCulling(bool cull); 
		void Refresh();
};