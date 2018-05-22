#include "RenderViewport.hpp"


#include "Util.hpp"

#include "TestGenerateVolume.hpp"

#include "IO/Image3DFromDicomFile.hpp"
#include "IO/Image3DFromTIFFFile.hpp"
#include "IO/Image3DFromPNGFile.hpp"


RenderViewport::RenderViewport()
{
	setFocusPolicy(Qt::ClickFocus);
	renderType = SLICE_RENDER; 
}

void RenderViewport::initializeGL()
{
	if(!isValid())
	{
		std::cout << "RenderViewport: QSurfaceFormat: surface failed" << std::endl; 
		return; 
	}
	
	OPENGL_FUNC_MACRO
	if(!ogl)
	{
		std::cout << "RenderViewport: ogl functions could not be loaded" << std::endl; 
		return; 
	}
	
    ogl->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	cameraObject = new CameraObject;
	cameraControl = new CameraControl(cameraObject);
	
	connect(cameraControl, &CameraControl::CameraUpdated, [this](){photonVolumeObject->ClearPhotonRender(windowWidth, windowHeight); update(); });
	
	TextureVolumeObject::InitSystem(); 
	textureVolumeObject = new TextureVolumeObject;
	textureVolumeObject->Init();
	
	RayVolumeObject::InitSystem();
	rayVolumeObject = new RayVolumeObject;
	rayVolumeObject->Init();
	
	PhotonVolumeObject::InitSystem();
	photonVolumeObject = new PhotonVolumeObject;
	photonVolumeObject->Init();
	
	textureVolumeObject->SetVisible(true); 
	rayVolumeObject->SetVisible(false); 
	photonVolumeObject->SetVisible(false); 
	
	textureVolume = new Texture3D; 
	
	textureGradient = new Texture3D;
	
	textureLUT = new Texture1D;
	
	textureEnvMap = new TextureCube; 
	
	//Load Default LUT texture
	int sizeLUT = 256; 
	std::vector<float> bufferLUT(sizeLUT * 4);
	for(int i = 0; i < sizeLUT; i++)
	{
		bufferLUT[i * 4 + 0] = (double)i / (double)sizeLUT;  
		bufferLUT[i * 4 + 1] = (double)i / (double)sizeLUT; 
		bufferLUT[i * 4 + 2] = (double)i / (double)sizeLUT; 
		bufferLUT[i * 4 + 3] = (double)i / (double)sizeLUT; 
	}
	LoadLUT(&bufferLUT[0], sizeLUT);
	
	LoadDefaultEnvMap();

	textureVolumeObject->SetVolumeTexture(textureVolume); 
	textureVolumeObject->SetGradientTexture(textureGradient); 
	textureVolumeObject->SetLUTTexture(textureLUT); 
	
	rayVolumeObject->SetVolumeTexture(textureVolume); 
	rayVolumeObject->SetGradientTexture(textureGradient); 
	rayVolumeObject->SetLUTTexture(textureLUT); 
	
	photonVolumeObject->SetVolumeTexture(textureVolume); 
	photonVolumeObject->SetGradientTexture(textureGradient); 
	photonVolumeObject->SetLUTTexture(textureLUT); 
	photonVolumeObject->SetEnvMap(textureEnvMap);
	
	axisObject = new AxisObject();
	AxisObject::InitSystem(); 
	axisObject->Init();
	axisObject->SetVisible(false);
	
	SetBackFaceCulling(true);
	
	//Update the viewport in real time if the photon volume object is in use
	QTimer* updateTimer = new QTimer();
	updateTimer->setInterval(1);
	connect(updateTimer, &QTimer::timeout, [this]()
	{
		if(photonVolumeObject->GetVisible()) update();
	});
	updateTimer->start(1);
}

void RenderViewport::resizeGL(int w, int h)
{
	windowWidth = w;
	windowHeight = h;
	photonVolumeObject->ClearPhotonRender(windowWidth, windowHeight);
}

void RenderViewport::paintGL()
{
	OPENGL_FUNC_MACRO
	if(!ogl)
	{
		std::cout << "RenderViewport: ogl functions could not be loaded" << std::endl; 
		return; 
	}
	

    ogl->glClear(GL_COLOR_BUFFER_BIT);
	ogl->glClear(GL_DEPTH_BUFFER_BIT);
	
	axisObject->Render(cameraObject->GetViewMatrix(), cameraObject->GetProjectionMatrix(windowWidth, windowHeight));
	textureVolumeObject->Render(cameraObject->GetViewMatrix(), cameraObject->GetProjectionMatrix(windowWidth, windowHeight));
	rayVolumeObject->Render(cameraObject->GetViewMatrix(), cameraObject->GetProjectionMatrix(windowWidth, windowHeight));
	photonVolumeObject->Render(cameraObject->GetViewMatrix(), cameraObject->GetProjectionMatrix(windowWidth, windowHeight));
	
	PrintGLErrors();
}

void RenderViewport::mousePressEvent(QMouseEvent *event)
{
	cameraControl->mousePressEvent(event);
}

void RenderViewport::mouseReleaseEvent(QMouseEvent *event)
{
	cameraControl->mouseReleaseEvent(event);
}

void RenderViewport::mouseMoveEvent(QMouseEvent *event)
{
	cameraControl->mouseMoveEvent(event); 
}

void RenderViewport::wheelEvent(QWheelEvent *event)
{
	cameraControl->wheelEvent(event); 
}

void RenderViewport::keyPressEvent(QKeyEvent *event)
{
	cameraControl->keyPressEvent(event);
}

void RenderViewport::keyReleaseEvent(QKeyEvent *event)
{
	cameraControl->keyReleaseEvent(event); 
}

void RenderViewport::EnableDisableAxis(bool en)
{
	axisObject->SetVisible(en);
}

void RenderViewport::ImportDicomFile(QString fileName)
{
	
}

void RenderViewport::ImportDicomFileSequence(QStringList fileNames)
{
	std::vector<std::string> files;
	for(int i = 0; i < fileNames.size(); i++)
		files.push_back(fileNames.at(i).toStdString());
	Image3D image3D;
	bool loadGood = Image3DFromDicomFileSequence(&image3D, files);
	if(!loadGood)
		return; 
	textureVolume->Allocate(image3D.Width(), image3D.Height(), image3D.Depth());
	textureVolume->LoadData(image3D.Data());
	
	photonVolumeObject->ClearPhotonRender(windowWidth, windowHeight);
	update();
}

void RenderViewport::ImportTIFFFileSequence(QStringList fileNames)
{
	std::vector<std::string> files;
	for(int i = 0; i < fileNames.size(); i++)
		files.push_back(fileNames.at(i).toStdString());
	
	Image3D image3D;
	bool loadGood = Image3DFromTIFFFileSequence(&image3D, files);
	if(!loadGood)
		return;
	
	
	Image3D IntensityImage;
	IntensityImage.Allocate(image3D.Width(), image3D.Height(), image3D.Depth(), 4); 
	IntensityImage.Copy(image3D); 
	IntensityImage.Smooth();
	IntensityImage.Median2D();
	textureVolume->Allocate(image3D.Width(), image3D.Height(), image3D.Depth(), false);
	textureVolume->LoadData(IntensityImage.Data());
	
	
	Image3D gradient3D;
	gradient3D.Allocate(image3D.Width(), image3D.Height(), image3D.Depth(), 3);
	//image3D.Smooth(); 
	image3D.Normalize();
	image3D.Median();
	//image3D.Median2D();
	
	//gradient3D.CentralDifference(image3D);
	gradient3D.Sobel(image3D);
	
	textureGradient->Allocate(gradient3D.Width(), gradient3D.Height(), gradient3D.Depth(), false, 3);
	textureGradient->LoadData(gradient3D.Data());
	
	photonVolumeObject->ClearPhotonRender(windowWidth, windowHeight);
	update();
}

void RenderViewport::LoadLUT(float* buffer, int sizeLUT)
{
	if(textureLUT->Width() != sizeLUT)
		textureLUT->Allocate(sizeLUT);
	textureLUT->LoadData(buffer);
	
	photonVolumeObject->ClearPhotonRender(windowWidth, windowHeight);
	update();
}

void RenderViewport::LoadDefaultEnvMap()
{
	uint64_t W = 1024;
	uint64_t H = 1024;
	Image3D posX(W, H, 1, 4);
	Image3D negX(W, H, 1, 4);
	Image3D posY(W, H, 1, 4);
	Image3D negY(W, H, 1, 4);
	Image3D posZ(W, H, 1, 4);
	Image3D negZ(W, H, 1, 4);
	unsigned char* posXData = (unsigned char*)posX.Data();
	unsigned char* negXData = (unsigned char*)negX.Data();
	unsigned char* posYData = (unsigned char*)posY.Data();
	unsigned char* negYData = (unsigned char*)negY.Data();
	unsigned char* posZData = (unsigned char*)posZ.Data();
	unsigned char* negZData = (unsigned char*)negZ.Data();
	
	///Draw a default scene with a floor and a light
	for(uint64_t y = 0; y < H; y++)
	{
		
				
		for(uint64_t x = 0; x < W; x++)
		{
			//gradient
			double minDist = 3; 
			double maxDist = 4; 
			double xx = 2 * ((double)x - (double)W / 2.0) / (double)W;
			double yy = 2 * ((double)y - (double)H / 2.0) / (double)H;			
			double zz = 1;
			double planeHeight = -1.0;
				
			glm::dvec3 ray = glm::normalize(glm::dvec3(xx, yy, zz));
			
			double frac = 1;
			if(ray.y > 0.001)
			{
				double dist;
				glm::dvec3 P;
				P = planeHeight / ray.y * ray;
				P.y = 0;
				dist = glm::length(P); 
				frac = (dist - minDist) / (maxDist - minDist); 
			}
		
				
			frac = frac < 0.0 ? 0.0 : frac;
			frac = frac > 1.0 ? 1.0 : frac;
		
			//Left Image
			posXData[(y * W + x) * 4] = (190 - 100) * frac + 100;
			posXData[(y * W + x) * 4 + 1] = (190 - 100) * frac + 100;
			posXData[(y * W + x) * 4 + 2] = (210 - 100) * frac + 100;
			posXData[(y * W + x) * 4 + 3] = 255;
			
			//Right Image
			negXData[(y * W + x) * 4] = (190 - 100) * frac + 100;
			negXData[(y * W + x) * 4 + 1] = (190 - 100) * frac + 100;
			negXData[(y * W + x) * 4 + 2] = (210 - 100) * frac + 100;
			negXData[(y * W + x) * 4 + 3] = 255;
			
			//Top Image
			posYData[(y * W + x) * 4] = 190;
			posYData[(y * W + x) * 4 + 1] = 190;
			posYData[(y * W + x) * 4 + 2] = 210;
			posYData[(y * W + x) * 4 + 3] = 255;
			
			//Bottom Image
			negYData[(y * W + x) * 4] = 100;
			negYData[(y * W + x) * 4 + 1] = 100;
			negYData[(y * W + x) * 4 + 2] = 100;
			negYData[(y * W + x) * 4 + 3] = 255;
			
			//Forward Image
			posZData[(y * W + x) * 4] = (190 - 100) * frac + 100;;
			posZData[(y * W + x) * 4 + 1] = (190 - 100) * frac + 100;;
			posZData[(y * W + x) * 4 + 2] = (210 - 100) * frac + 100;
			posZData[(y * W + x) * 4 + 3] = 255;
			
			//Back Image
			negZData[(y * W + x) * 4] = (190 - 100) * frac + 100;;
			negZData[(y * W + x) * 4 + 1] = (190 - 100) * frac + 100;;
			negZData[(y * W + x) * 4 + 2] = (210 - 100) * frac + 100;
			negZData[(y * W + x) * 4 + 3] = 255;
		}
	}
	
	textureEnvMap->Allocate(W, H, false, 4);
	textureEnvMap->LoadDataXPos(posXData);
	textureEnvMap->LoadDataXNeg(negXData);
	textureEnvMap->LoadDataYPos(posYData);
	textureEnvMap->LoadDataYNeg(negYData);
	textureEnvMap->LoadDataZPos(posZData);
	textureEnvMap->LoadDataZNeg(negZData);
	
	photonVolumeObject->ClearPhotonRender(windowWidth, windowHeight);
	update();
}

void RenderViewport::ChooseRenderer(RENDER_TYPE rt)
{
	renderType = rt; 
	if(renderType == SLICE_RENDER)
	{
		textureVolumeObject->SetVisible(true); 
		rayVolumeObject->SetVisible(false); 
		photonVolumeObject->SetVisible(false); 
	}
	else if(renderType == RAY_RENDER)
	{
		textureVolumeObject->SetVisible(false); 
		rayVolumeObject->SetVisible(true); 
		photonVolumeObject->SetVisible(false); 
	}
	else if(renderType == PHOTON_RENDER)
	{
		textureVolumeObject->SetVisible(false); 
		rayVolumeObject->SetVisible(false); 
		photonVolumeObject->SetVisible(true); 
	}
	
	photonVolumeObject->ClearPhotonRender(windowWidth, windowHeight);
	update();
}

void RenderViewport::SetGradientThreshold(float threshold)
{
	rayVolumeObject->SetGradientThreshold(threshold);
	photonVolumeObject->SetGradientThreshold(threshold);
	
	photonVolumeObject->ClearPhotonRender(windowWidth, windowHeight);
	update();
}

void RenderViewport::SetBackFaceCulling(bool cull)
{
	rayVolumeObject->SetBackFaceCulling(cull);
	photonVolumeObject->SetBackFaceCulling(cull);
	
	photonVolumeObject->ClearPhotonRender(windowWidth, windowHeight);
	update();
}