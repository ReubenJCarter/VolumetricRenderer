#pragma once


#include "Object3D.hpp"
#include "Texture3D.hpp"
#include "TextureCube.hpp"
#include "Texture1D.hpp"


class PhotonVolumeObject: public Object3D
{
	protected:
		struct Vertex
		{
			float x;
			float y;
			float z;
			float w;
		};
		
		struct Triangle
		{
			uint32_t v0;
			uint32_t v1;
			uint32_t v2;
		};		
		
		static std::string vertSrc;
		static std::string fragSrc; 
		static int programShaderObject;
		static int vertexShaderObject;
		static int fragmentShaderObject;
		static std::string displayVertSrc; 
		static std::string displayFragSrc; 
		static int displayProgramShaderObject;
		static int displayVertexShaderObject;
		static int displayFragmentShaderObject;
		
		
		Texture1D* lutTexture; 
		
		Texture3D* volumeTexture; 
		Texture3D* gradientTexture; 
		TextureCube* envMapTexture;
		
		unsigned int volumeSlices;
		
		unsigned int vertexBuffer;
		unsigned int vertexArrayObject;
		
		std::mt19937 randGenerator;
		unsigned int randomBuffer; 
		unsigned int randomNumberBindingPoint;
		
		unsigned int frameBuffer;
		unsigned int frameBufferColorBuffer;
		unsigned int frameBufferDepthBuffer;
		
		unsigned int targetWidth;
		unsigned int targetHeight;
		
		bool clearFlag; 
		int currentSampleNumber;
		int maxBounce; 
		int sampleCount;
		float brightness;
		float contrast;
		float gradientThreshold;
		bool backFaceCulling; 
		
	
	public:
		static void InitSystem();
		
		PhotonVolumeObject();
		virtual void Init();
		virtual void Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
		virtual void Destroy();
		
		void ClearPhotonRender(int W, int H);
		
		void SetVolumeTexture(Texture3D* vt);
		void SetGradientTexture(Texture3D* gt);
		void SetLUTTexture(Texture1D* lt);
		void SetEnvMap(TextureCube* env);
		void SetGradientThreshold(float gt);
		void SetBackFaceCulling(bool cull);
};