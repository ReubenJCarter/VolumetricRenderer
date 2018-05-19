#include "PhotonVolumeObject.hpp"


#include "Util.hpp"


std::string PhotonVolumeObject::vertSrc= R"(
#version 330

//attribs
layout(location = 0) in vec4 pointPosition;

//transforms
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix; 

//outputs
out vec3 rayOrig;
out vec3 rayDir; 

//main
void main()
{
	//compute rays
	vec4 frust = inverse(projectionMatrix) * vec4(pointPosition.x, pointPosition.y, 1.0f, 1.0f);
	frust.w = 0; 
	frust = normalize(frust); 
	
	//compute outputs
	rayOrig = (inverse(viewMatrix) * vec4(0, 0, 0, 1)).xyz; 
	rayDir = normalize(inverse(viewMatrix) * frust).xyz;
	gl_Position = vec4(pointPosition.x, pointPosition.y, pointPosition.z, 1.0f);
}
)";
 
 
 
 //
//Photon mapping frag shader
//

std::string PhotonVolumeObject::fragSrc = R"(
#version 330

#define PI 3.14159265359

//inputs
in vec3 rayOrig;
in vec3 rayDir;

//uniforms
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform sampler3D volumeTexture;
uniform sampler3D gradientTexture;
uniform samplerCube envMapTexture;
uniform vec3 texDim;
uniform float brightness;
uniform float contrast;
uniform float gradientThreshold;
uniform int backFaceCulling; 
uniform sampler1D lutTexture;

//output
layout(location = 0) out vec4 outputColor; 

//Ray intersects
struct HitInfo
{
	bool hit; 
	float dist; 
};

#define BIGNUM 1e10
HitInfo RayAABBIntersect(vec3 rayOrig, vec3 rayDirInv, vec3 minaabb, vec3 maxaabb, float dist)
{
	HitInfo hitInfo;
	vec3 t1 = (minaabb - rayOrig) * rayDirInv;
	vec3 t2 = (maxaabb - rayOrig) * rayDirInv;
	float tmin = max(max(min(t1.x, t2.x), min(t1.y, t2.y)), min(t1.z, t2.z));
	float tmax = min(min(max(t1.x, t2.x), max(t1.y, t2.y)), max(t1.z, t2.z));
	hitInfo.hit = tmax >= max(0.0f, tmin) && tmin < dist;
	hitInfo.dist = tmin; 
	return hitInfo;
}

//Random
uint Hash(uint x)
{
	x += (x << 10u);
	x ^= (x >> 6u);
	x += (x << 3u);
	x ^= (x >> 11u);
	x += (x << 15u);
	return x;
}

uint Hash(uvec3 v)
{
	return Hash(v.x ^ Hash(v.y) ^ Hash(v.z));
}

float Random(vec2 f, float seed)
{
	const uint mantissaMask = 0x007FFFFFu;
	const uint one = 0x3F800000u;
	uint h = Hash(floatBitsToUint(vec3(f, seed)));
	h &= mantissaMask;
	h |= one;
	float r2 = uintBitsToFloat(h);
	return r2 - 1.0;
}

vec2 Random(vec2 f, vec2 seed)
{
	return vec2(Random(f, seed.x), Random(f, seed.y));
}

vec3 Random(vec2 f, vec3 seed)
{
	return vec3(Random(f, seed.x), Random(f, seed.y), Random(f, seed.z));
}

vec3 RandomUnitHemi(vec2 randomVal, vec3 norm)
{
	float a = (randomVal.x + 1.0) * PI;
	float u = randomVal.y;
	float u2 = u * u;
	float sqrt1MinusU2 = sqrt(1.0 - u2);
	float x = sqrt1MinusU2 * cos(a);
	float y = sqrt1MinusU2 * sin(a);
	float z = u;
 	vec3 rh = vec3(x, y, z);
	return rh * sign(dot(rh, norm));
}

//3d Volume Fetch
vec4 Fetch3DVolume(vec3 position)
{
	float hasp = texDim.x / texDim.y;
	float dasp = texDim.z / texDim.y;
	return texture(volumeTexture, (position.xyz * vec3(1, 1, 1/dasp) + vec3(0.5f, 0.5f, 0.5f)));
}

vec3 FetchGradient(vec3 position)
{
	float hasp = texDim.x / texDim.y;
	float dasp = texDim.z / texDim.y;
	return texture(gradientTexture, (position.xyz * vec3(1, 1, 1/dasp) + vec3(0.5f, 0.5f, 0.5f))).xyz - vec3(0.5f, 0.5f, 0.5f);
}

vec4 FetchEnvMap(vec3 dir)
{
	return texture(envMapTexture, dir);
}

//main
void main()
{
	vec3 rayDirNorm = normalize(rayDir); 
	vec3 rayDirInv = vec3(1, 1, 1) / rayDirNorm; 

	HitInfo hit = RayAABBIntersect(rayOrig, rayDirInv, vec3(-0.5, -0.5, -0.5), vec3(0.5, 0.5, 0.5), BIGNUM);
	

	float stepSize = 0.002f;
	
	vec3 sampleColor = vec3(0, 0, 0);
	
	int sampleNumber = 100; 
	
	int photonMarchCount = 800;
	
	if(hit.hit)
	{
	
		for(int s = 0; s < sampleNumber; s++)
		{
		
		vec3 finalColor = vec3(0, 0, 0);
		vec3 runningReflectanceFactor = vec3(1.0f, 1.0f, 1.0f);
		
		vec3 photonDir = rayDirNorm; 
		vec3 photonPos = rayOrig + rayDirNorm * (hit.dist + stepSize + Random(gl_FragCoord.xy, 1234) * stepSize);
		
		for(int i = 0; i < photonMarchCount; i++)
		{
			
			vec3 gradient = FetchGradient(photonPos);
			float gradientLen = length(gradient);
					
			if(gradientLen > gradientThreshold)
			{
				vec3 gradientNorm = normalize(gradient);
				if(dot(-photonDir, gradientNorm) > 0 || !bool(backFaceCulling))
				{
					vec4 col = Fetch3DVolume(photonPos + -gradientNorm * stepSize * 3);
					vec4 surface = texture(lutTexture, col.r);
					vec3 surfacecol = surface.xyz; 
					float surfaceopacity = surface.w;
					
					
					vec2 randomVec2 = vec2(float(i * sampleNumber + s) / float(sampleNumber * photonMarchCount),
										   float(s * photonMarchCount + i) / float(sampleNumber * photonMarchCount));
					vec3 newRayD = RandomUnitHemi(Random(gl_FragCoord.xy, randomVec2) * 2.0f - vec2(1.0f, 1.0f), gradientNorm);
					vec3 reflectanceFactor = max(0.0f, dot(newRayD, gradientNorm)) * surface.xyz;
					runningReflectanceFactor *= reflectanceFactor;
					
					photonDir = newRayD;
				}
			}
					
			photonPos += photonDir * stepSize;
			
			if(photonPos.x > 0.5f || photonPos.y > 0.5f || photonPos.z > 0.5f || 
			   photonPos.x < -0.5f || photonPos.y < -0.5f || photonPos.z < -0.5f)
			{
				vec4 backgroundTex = FetchEnvMap(photonDir);
				vec3 materialEmittance = backgroundTex.xyz;
				finalColor += runningReflectanceFactor * materialEmittance;
				break; 
			}
		}
		
		sampleColor += finalColor;
		
		}
	}
	else
	{
		vec4 backgroundTex = FetchEnvMap(rayDirNorm);
		vec3 materialEmittance = backgroundTex.xyz * 1.0f;
		sampleColor = backgroundTex.xyz;
		sampleNumber= 1;
	}
	
	sampleColor *= 1.0f / float(sampleNumber);//average
	
	outputColor = vec4(sampleColor.x, sampleColor.y, sampleColor.z, 1.0f);
}
)";
 
int PhotonVolumeObject::programShaderObject;
int PhotonVolumeObject::vertexShaderObject;
int PhotonVolumeObject::fragmentShaderObject;
 
void PhotonVolumeObject::InitSystem()
{
	OPENGL_FUNC_MACRO
	
	//make array to pointer for source code (needed for opengl )
	const char* vsrc[1];
	const char* fsrc[1];
	vsrc[0] = vertSrc.c_str();
	fsrc[0] = fragSrc.c_str();
	
	//compile vertex and fragment shaders from source
	vertexShaderObject = ogl->glCreateShader(GL_VERTEX_SHADER);
	ogl->glShaderSource(vertexShaderObject, 1, vsrc, NULL);
	ogl->glCompileShader(vertexShaderObject);
	fragmentShaderObject = ogl->glCreateShader(GL_FRAGMENT_SHADER);
	ogl->glShaderSource(fragmentShaderObject, 1, fsrc, NULL);
	ogl->glCompileShader(fragmentShaderObject);
	
	//link vertex and fragment shader to create shader program object
	programShaderObject = ogl->glCreateProgram();
	ogl->glAttachShader(programShaderObject, vertexShaderObject);
	ogl->glAttachShader(programShaderObject, fragmentShaderObject);
	ogl->glLinkProgram(programShaderObject);
	
	//Check status of shader and log any compile time errors
	int linkStatus;
	ogl->glGetProgramiv(programShaderObject, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE) 
	{
		char log[5000];
		int logLen; 
		ogl->glGetProgramInfoLog(programShaderObject, 5000, &logLen, log);
		std::cerr << "TextureVolumeObject:Could not link program: " << std::endl;
		std::cerr << log << std::endl;
		ogl->glGetShaderInfoLog(vertexShaderObject, 5000, &logLen, log);
		std::cerr << "vertex shader log:\n" << log << std::endl;
		ogl->glGetShaderInfoLog(fragmentShaderObject, 5000, &logLen, log);
		std::cerr << "fragment shader log:\n" << log << std::endl;
		ogl->glDeleteProgram(programShaderObject);
		programShaderObject = 0;
	}
	else
	{
		std::cout << "PhotonVolumeObject::CompileShader:compile success " << std::endl;
	}
}


PhotonVolumeObject::PhotonVolumeObject()
{
	randomNumberBindingPoint = 0;
	maxBounce = 20;
	sampleCount = 100;
	brightness = 0;
	contrast = 1;
	gradientThreshold = 0.06;
	backFaceCulling = true; 
}


void PhotonVolumeObject::Init()
{
	//Init Buffers and VAO for rendering	
	OPENGL_FUNC_MACRO

	ogl->glGenBuffers(1, &vertexBuffer);
	ogl->glGenVertexArrays(1, &vertexArrayObject);
	
	//seed Random number generator
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	randGenerator.seed(seed);
	ogl->glGenBuffers(1, &randomBuffer);
	ogl->glBindBuffer(GL_UNIFORM_BUFFER, randomBuffer);
	ogl->glBindBufferBase(GL_UNIFORM_BUFFER, randomNumberBindingPoint, randomBuffer);
	ogl->glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	//Bind VAO
	ogl->glBindVertexArray(vertexArrayObject);
	
	//build buffers
	std::vector<Vertex> vertexData(6); 
	
	Vertex v;
	v.w = 1.0;
	
	
	double extent = 1.0; 
	
	v.z = 0; 
	v.x = -extent;
	v.y = -extent;
	vertexData[0] = v;
	v.x = extent;
	v.y = -extent;
	vertexData[1] = v;
	v.x = extent;
	v.y = extent;
	vertexData[2] = v;
	v.x = extent;
	v.y = extent;
	vertexData[3] = v;
	v.x = -extent;
	v.y = extent;
	vertexData[4] = v;
	v.x = -extent;
	v.y = -extent;
	vertexData[5] = v;

	
	
	ogl->glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	ogl->glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), (char*)&vertexData[0], GL_STATIC_DRAW);

	//set vertex attributes
	ogl->glEnableVertexAttribArray(0);
	ogl->glVertexAttribPointer(0, 4, GL_FLOAT, false, sizeof(Vertex), (void*)((uintptr_t)0));
	
	//Unbind VAO
	ogl->glBindVertexArray(0);
	
	//Unbind array and element buffers
	ogl->glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	volumeTexture = NULL; 
	
	lutTexture = NULL; 
	
	envMapTexture = NULL; 
}


void PhotonVolumeObject::Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	if(!visible) return; 
	
	std::cout << "PhotonVolumeObject: Rendering" << std::endl; 
	
	OPENGL_FUNC_MACRO
	
	//compute mvp matrix
	glm::mat4 modelMatrix = GetModelMatrix(); 
	
	//init random buffer
	std::uniform_real_distribution<float> randDist(0.0f, 1.0f);
	std::vector<float> randVec(maxBounce * sampleCount * 2);
	for(int i = 0; i < maxBounce * sampleCount; i++)
	{
		randVec[i * 2 + 0] = randDist(randGenerator);
		randVec[i * 2 + 1] = randDist(randGenerator);
	}
	ogl->glBindBuffer(GL_UNIFORM_BUFFER, randomBuffer);
	ogl->glBufferData(GL_UNIFORM_BUFFER, randVec.size() * sizeof(float), &randVec[0], GL_STATIC_DRAW);
	ogl->glBindBuffer(GL_UNIFORM_BUFFER, 0);
	int randomBufferBlockIndex = ogl->glGetUniformBlockIndex(programShaderObject, "randomNumbers");
	ogl->glUniformBlockBinding(programShaderObject, randomBufferBlockIndex, randomNumberBindingPoint);
	
	//bind shader
	ogl->glUseProgram(programShaderObject);
	
	//update mvp transform uniform in shader
	int projectionMatrixLocation = ogl->glGetUniformLocation(programShaderObject, "projectionMatrix"); 
	ogl->glUniformMatrix4fv(projectionMatrixLocation, 1, false, glm::value_ptr(projectionMatrix));
	
	int modelMatrixLocation = ogl->glGetUniformLocation(programShaderObject, "modelMatrix"); 
	ogl->glUniformMatrix4fv(modelMatrixLocation, 1, false, glm::value_ptr(modelMatrix));
	
	int viewMatrixLocation = ogl->glGetUniformLocation(programShaderObject, "viewMatrix"); 
	ogl->glUniformMatrix4fv(viewMatrixLocation, 1, false, glm::value_ptr(viewMatrix));
	
	
	//update 3d texture volume
	int texDimLocation = ogl->glGetUniformLocation(programShaderObject, "texDim"); 
	ogl->glUniform3f(texDimLocation, (float)volumeTexture->Width(), (float)volumeTexture->Height(), (float)volumeTexture->Depth());
	int volumeTextureLocation = ogl->glGetUniformLocation(programShaderObject, "volumeTexture"); 
	ogl->glUniform1i(volumeTextureLocation, 0);
	ogl->glActiveTexture(GL_TEXTURE0 + 0);
	if(volumeTexture != NULL)
	{
		ogl->glBindTexture(GL_TEXTURE_3D, volumeTexture->GetTextureId());
		
		ogl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		ogl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		ogl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		ogl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		ogl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
		float bcolor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		ogl->glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, bcolor);
	}
	else
	{
		ogl->glBindTexture(GL_TEXTURE_3D, 0);
	}
	
	//Update 3d texture gradient
	int gradientTextureLocation = ogl->glGetUniformLocation(programShaderObject, "gradientTexture"); 
	ogl->glUniform1i(gradientTextureLocation, 1);
	ogl->glActiveTexture(GL_TEXTURE0 + 1);
	if(gradientTexture != NULL)
	{
		ogl->glBindTexture(GL_TEXTURE_3D, gradientTexture->GetTextureId());
		
		ogl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		ogl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		ogl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		ogl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		ogl->glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
		float bcolor[] = { 0.5f, 0.5f, 0.5f, 0.0f };
		ogl->glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, bcolor);
	}
	else
	{
		ogl->glBindTexture(GL_TEXTURE_3D, 0);
		std::cout << "PhotonVolumeObject:Render:gradient texture NULL" << std::endl; 
	}
	
	//update LUT texture
	int lutTextureLocation = ogl->glGetUniformLocation(programShaderObject, "lutTexture"); 
	ogl->glUniform1i(lutTextureLocation, 2);
	ogl->glActiveTexture(GL_TEXTURE0 + 2);
	
	if(lutTexture != NULL)
	{
		ogl->glBindTexture(GL_TEXTURE_1D, lutTexture->GetTextureId());
		
		ogl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		ogl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		ogl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		ogl->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		ogl->glBindTexture(GL_TEXTURE_1D, 0);
	}
	
	//update env map texture
	int envMapTextureLocation = ogl->glGetUniformLocation(programShaderObject, "envMapTexture"); 
	ogl->glUniform1i(envMapTextureLocation, 3);
	ogl->glActiveTexture(GL_TEXTURE0 + 3);
	
	if(envMapTexture != NULL)
	{
		ogl->glBindTexture(GL_TEXTURE_CUBE_MAP, envMapTexture->GetTextureId());
		
		ogl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		ogl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		ogl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		ogl->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		ogl->glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	
	//update material uniforms
	int materialAlphaLocation = ogl->glGetUniformLocation(programShaderObject, "brightness"); 
	ogl->glUniform1f(materialAlphaLocation, brightness);
	int materialPointSizeLocation = ogl->glGetUniformLocation(programShaderObject, "contrast"); 
	ogl->glUniform1f(materialPointSizeLocation, contrast);
	int materialGradientThresholdLocation = ogl->glGetUniformLocation(programShaderObject, "gradientThreshold"); 
	ogl->glUniform1f(materialGradientThresholdLocation, gradientThreshold);
	int materialBackFaceCullingLocation = ogl->glGetUniformLocation(programShaderObject, "backFaceCulling"); 
	ogl->glUniform1i(materialBackFaceCullingLocation, (int)backFaceCulling);
	
	//bind VAO
	ogl->glBindVertexArray(vertexArrayObject);
	
	//draw elements
	ogl->glDrawArrays(GL_TRIANGLES, 0, 6);
	
	
	//unbind VAO
	ogl->glBindVertexArray(0);
	
	//unbind shader program
	ogl->glUseProgram(0);
	

}


void PhotonVolumeObject::Destroy()
{
	OPENGL_FUNC_MACRO
	ogl->glDeleteBuffers(1, &vertexBuffer);
	ogl->glDeleteVertexArrays(1, &vertexArrayObject);
}


void PhotonVolumeObject::SetVolumeTexture(Texture3D* vt)
{
	volumeTexture = vt; 
}


void PhotonVolumeObject::SetGradientTexture(Texture3D* gt)
{
	gradientTexture = gt;
}


void PhotonVolumeObject::SetLUTTexture(Texture1D* lt)
{
	lutTexture = lt;
}

void PhotonVolumeObject::SetEnvMap(TextureCube* env)
{
	envMapTexture = env; 
}

void PhotonVolumeObject::SetGradientThreshold(float gt)
{
	gradientThreshold = gt;
}

void PhotonVolumeObject::SetBackFaceCulling(bool cull)
{
	backFaceCulling = cull;
}
