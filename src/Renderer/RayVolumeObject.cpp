#include "RayVolumeObject.hpp"


#include "Util.hpp"


std::string RayVolumeObject::vertSrc= R"(
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
 
std::string RayVolumeObject::fragSrc = R"(
#version 330

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
		 		
		float minGrad = 0.06f;
		if(gradientLen > minGrad)
		{
			vec4 surface = texture(lutTexture, col.r);
			vec3 surfacecol = surface.xyz; 
			float surfaceopacity = surface.w;
			
			vec3 gradientNorm = normalize(gradient);
			
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
				
		
		rayStart += rayDirNorm * stepSize;
	}
	
	
	outputColor = finalColor;
}
)";
 
int RayVolumeObject::programShaderObject;
int RayVolumeObject::vertexShaderObject;
int RayVolumeObject::fragmentShaderObject;
 
void RayVolumeObject::InitSystem()
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
		std::cout << "RayVolumeObject::CompileShader:compile success " << std::endl;
	}
}


RayVolumeObject::RayVolumeObject()
{
	brightness = 0;
	contrast = 1;
}


void RayVolumeObject::Init()
{
	//Init Buffers and VAO for rendering	
	OPENGL_FUNC_MACRO

	ogl->glGenBuffers(1, &vertexBuffer);
	ogl->glGenVertexArrays(1, &vertexArrayObject);
	
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


void RayVolumeObject::Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	if(!visible) return; 
	
	std::cout << "RayVolumeObject: Rendering" << std::endl; 
	
	OPENGL_FUNC_MACRO
	
	//compute mvp matrix
	glm::mat4 modelMatrix = GetModelMatrix(); 
	
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
		std::cout << "RayVolumeObject:Render:gradient texture NULL" << std::endl; 
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
	
	
	//bind VAO
	ogl->glBindVertexArray(vertexArrayObject);
	
	//draw elements
	ogl->glDrawArrays(GL_TRIANGLES, 0, 6);
	
	
	//unbind VAO
	ogl->glBindVertexArray(0);
	
	//unbind shader program
	ogl->glUseProgram(0);
	

}


void RayVolumeObject::Destroy()
{
	OPENGL_FUNC_MACRO
	ogl->glDeleteBuffers(1, &vertexBuffer);
	ogl->glDeleteVertexArrays(1, &vertexArrayObject);
}


void RayVolumeObject::SetVolumeTexture(Texture3D* vt)
{
	volumeTexture = vt; 
}


void RayVolumeObject::SetGradientTexture(Texture3D* gt)
{
	gradientTexture = gt;
}


void RayVolumeObject::SetLUTTexture(Texture1D* lt)
{
	lutTexture = lt;
}
