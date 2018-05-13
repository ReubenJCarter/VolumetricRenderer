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

//main
void main()
{
	vec3 rayDirNorm = normalize(rayDir); 
	vec3 rayDirInv= vec3(1, 1, 1) / rayDirNorm; 
	vec3 lightDir0 = vec3(0.707f, -0.707f, 0);
	vec3 lightDir1 = vec3(-0.707f, 0, 0.707f);
	vec3 lightDir2 = vec3(0, 0.707f, -0.707f);
	
	HitInfo hi = RayAABBIntersect(rayOrig, rayDirInv, vec3(-0.5, -0.5, -0.5), vec3(0.5, 0.5, 0.5), BIGNUM);
	
	if(!hi.hit)
		discard;
	
	float stepSize = 0.002f;
	vec3 rayStart = rayOrig + rayDirNorm * (hi.dist + stepSize + Random(gl_FragCoord.xy, 1234) * stepSize);
	
	vec4 finalColor = vec4(0, 0, 0, 0);
	
	
	for(int i = 0; i < 500; i++)
	{
		vec4 col = Fetch3DVolume(rayStart);
		vec3 gradient = FetchGradient(rayStart);
		float gradientLen = length(gradient);
		 		
		if(gradientLen > gradientThreshold)
		{
			vec3 gradientNorm = normalize(gradient);
			if(dot(-rayDirNorm, gradientNorm) > 0 || !bool(backFaceCulling))
			{
				vec4 surface = texture(lutTexture, col.r);
				vec3 surfacecol = surface.xyz; 
				float surfaceopacity = surface.w;
				
				
				
				float diffuse = 0.5 * max(0, dot(gradientNorm, lightDir0)) + 
								0.5 * max(0, dot(gradientNorm, lightDir1)) +
								0.2 * max(0, dot(gradientNorm, lightDir2));
				float ambient = 0.2f;
				
				
				vec3 reflection0 = 2 * max(0, dot(lightDir0, gradientNorm)) * gradientNorm - lightDir0; 
				vec3 reflection1 = 2 * max(0, dot(lightDir1, gradientNorm)) * gradientNorm - lightDir1; 
				vec3 reflection2 = 2 * max(0, dot(lightDir2, gradientNorm)) * gradientNorm - lightDir2; 
				
				float specular = 0.3 * pow(max(0, dot(reflection0, -rayDirNorm)), 30) + 
								 0.3 * pow(max(0, dot(reflection1, -rayDirNorm)), 30) +
								 0.3 * pow(max(0, dot(reflection2, -rayDirNorm)), 30);
								 
				vec3 phong  = ambient * surfacecol + diffuse * surfacecol + specular * vec3(1, 1, 1);
				finalColor = vec4(phong.x, phong.y, phong.z, 1.0f);
				break; 
			}
		}
				
		
		rayStart += rayDirNorm * stepSize;
		
		if(rayStart.x > 0.5 || rayStart.y > 0.5 || rayStart.z > 0.5 || rayStart.x < -0.5 || rayStart.y < -0.5 || rayStart.z < -0.5)
			break; 
	}
	
	
	outputColor = finalColor;
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
	}
	else
	{
		ogl->glBindTexture(GL_TEXTURE_1D, 0);
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

void PhotonVolumeObject::SetGradientThreshold(float gt)
{
	gradientThreshold = gt;
}

void PhotonVolumeObject::SetBackFaceCulling(bool cull)
{
	backFaceCulling = cull;
}
