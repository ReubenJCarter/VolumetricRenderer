#include "MeshObject.hpp"


#include "Util.hpp"


std::string MeshObject::vertSrc= R"(
#version 330
//attribs
layout(location = 0) in vec4 pointPosition;
layout(location = 1) in vec4 pointColor;

//transforms
uniform mat4 modelViewProjectionMatrix;

//outputs
out vec4 fragColor;

//main
void main()
{
	//compute outputs
	fragColor = pointColor; 
	gl_Position = modelViewProjectionMatrix * vec4(pointPosition.x, pointPosition.y, pointPosition.z, 1.0f);
}
)";
 
std::string MeshObject::fragSrc = R"(
#version 330
//inputs
in vec4 fragColor;

//uniforms

//output
layout(location = 0) out vec4 outputColor;

//main
void main()
{
  	outputColor = fragColor;
}
)";
 
int MeshObject::programShaderObject;
int MeshObject::vertexShaderObject;
int MeshObject::fragmentShaderObject;
 
void MeshObject::InitSystem()
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
		std::cerr << "AxisObject:Could not link program: " << std::endl;
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
		std::cout << "MeshObject::CompileShader:compile success " << std::endl;
	}
}


MeshObject::MeshObject()
{
	
}


void MeshObject::Init()
{
	//Init Buffers and VAO for rendering	
	OPENGL_FUNC_MACRO
	
	ogl->glGenBuffers(1, &vertexBuffer);
	ogl->glGenBuffers(1, &elementBuffer);
	ogl->glGenVertexArrays(1, &vertexArrayObject);
	
	//Bind VAO
	ogl->glBindVertexArray(vertexArrayObject);
	
	ogl->glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	ogl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

	//set vertex attributes
	ogl->glEnableVertexAttribArray(0);
	ogl->glVertexAttribPointer(0, 4, GL_FLOAT, false, sizeof(Vertex), (void*)((uintptr_t)0));
	ogl->glEnableVertexAttribArray(1);
	ogl->glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex), (void*)((uintptr_t)(0 + sizeof(float) * 4 )));
	
	//Unbind VAO
	ogl->glBindVertexArray(0);
	
	//Unbind array and element buffers
	ogl->glBindBuffer(GL_ARRAY_BUFFER, 0);
	ogl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void MeshObject::Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	if(!visible) return; 
	
	std::cout << "Rendering Axis" << std::endl; 
	
	OPENGL_FUNC_MACRO
	
	//compute mvp matrix
	glm::mat4 modelMatrix = GetModelMatrix(); 
	glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
		
	ogl->glEnable(GL_DEPTH_TEST);
	
	//enable backface culling 
	ogl->glEnable(GL_CULL_FACE); 
	
	//enable blending
	ogl->glEnable(GL_BLEND);
	ogl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ZERO, GL_SRC_COLOR);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	//bind shader
	ogl->glUseProgram(programShaderObject);
	
	//update mvp transform uniform in shader
	int modelViewProjectionMatrixLocation = ogl->glGetUniformLocation(programShaderObject, "modelViewProjectionMatrix"); 
	ogl->glUniformMatrix4fv(modelViewProjectionMatrixLocation, 1, false, glm::value_ptr(mvpMatrix));
	
	//bind VAO
	ogl->glBindVertexArray(vertexArrayObject);
	
	//draw elements
//	ogl->glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
	ogl->glDrawElements(GL_TRIANGLES, elementNumber, GL_UNSIGNED_INT, 0);
	//ogl->glDrawArrays(GL_TRIANGLES, 0, volumeSlices * 6);
	
	//unbind VAO
	ogl->glBindVertexArray(0);
	
	//unbind shader program
	ogl->glUseProgram(0);
	
	PrintGLErrors();
}


void MeshObject::Destroy()
{
	OPENGL_FUNC_MACRO
	
	ogl->glDeleteBuffers(1, &vertexBuffer);
	ogl->glDeleteBuffers(1, &elementBuffer);
	ogl->glDeleteVertexArrays(1, &vertexArrayObject);
}

void MeshObject::UpdateMeshBuffer()
{
	OPENGL_FUNC_MACRO
	
	ogl->glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	ogl->glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(Vertex), (char*)&verticies[0], GL_STATIC_DRAW);
	ogl->glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	ogl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	ogl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), (char*)&elements[0], GL_STATIC_DRAW);
	ogl->glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	aabbMin.x = std::numeric_limits<float>::max();
	aabbMax.x = std::numeric_limits<float>::lowest();
	aabbMin.y = std::numeric_limits<float>::max();
	aabbMax.y = std::numeric_limits<float>::lowest();
	aabbMin.z = std::numeric_limits<float>::max();
	aabbMax.z = std::numeric_limits<float>::lowest();
	for(int i = 0; i < verticies.size(); i++)
	{
		if(verticies[i].x < aabbMin.x)aabbMin.x = verticies[i].x;
		if(verticies[i].x > aabbMax.x)aabbMax.x = verticies[i].x;
		if(verticies[i].y < aabbMin.y)aabbMin.y = verticies[i].y;
		if(verticies[i].y > aabbMax.y)aabbMax.y = verticies[i].y;
		if(verticies[i].z < aabbMin.z)aabbMin.z = verticies[i].z;
		if(verticies[i].z > aabbMax.z)aabbMax.z = verticies[i].z;
	}
}
