#include "Application2.h"

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "Gizmos.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Camera.h"
#include "Vertex.h"

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "tiny_obj_loader.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

class Camera;
class FlyCamera;
struct Vertex;

Application2::Application2()
{}

int Application2::Run()
{
	if(glfwInit() == false)
		return -1;

	m_window = glfwCreateWindow(1280, 720, "Computer Graphics", nullptr, nullptr);

	if(m_window == nullptr) {
		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(m_window);

	if(ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return -3;
	}

	Gizmos::create();
	
	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n",major,minor);

	glClearColor(0.25f, 0.25f, 1.0f, 1);
	glEnable(GL_DEPTH_TEST);
	
	float previousTime = 0.0f;

	Load();

	while(glfwWindowShouldClose(m_window) == false && glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {

		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - previousTime;
		previousTime = currentTime;
		
		Update(deltaTime);
		Draw();

		Gizmos::draw(m_camera->GetProjectionView());

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	delete m_meshArray;
	delete m_camera;

	Gizmos::destroy();

	glfwDestroyWindow(m_window);
	glfwTerminate();
	return 0;
}

void Application2::Update(float deltaTime)
{
	m_camera->Update(deltaTime);
}

void Application2::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glUseProgram(m_programID);
	//unsigned int projectionViewUniform = glGetUniformLocation(m_programID, "ProjectionView");
	//unsigned int timeUniform = glGetUniformLocation(m_programID, "time");
	//unsigned int heightScaleUniform = glGetUniformLocation(m_programID, "heightScale");
	//
	//glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_camera->GetProjectionView()));
	//glUniform1f(timeUniform, (float)glfwGetTime());
	//float heightScale = 1.0f;
	//glUniform1f(heightScaleUniform, heightScale);
	//
	//m_meshArray->resetIterator();
	//for(unsigned int i = 0; i < 2/*m_meshArray->GetMeshCount()*/; i++)
	//{
	//	glBindVertexArray(m_meshArray->GetVAO());
	//	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //
	//	//glDrawElements(GL_TRIANGLES, m_meshArray->GetIndexCount(), GL_UNSIGNED_INT, 0);
	//	m_meshArray->Iterate();
	//}
	//
	//
	////Texture shader stuff
	//glUseProgram(m_textureProgramID);
	//
	//int loc = glGetUniformLocation(m_textureProgramID, "ProjectionView");
	//glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_camera->GetProjectionView()[0][0]));
	//
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_texture);
	//
	//loc = glGetUniformLocation(m_textureProgramID, "diffuse");
	//glUniform1i(loc, 0);
	//
	//glBindVertexArray(m_meshArray->GetVAO());
	//glDrawElements(GL_TRIANGLES, m_meshArray->GetIndexCount(), GL_UNSIGNED_INT, 0);
	//m_meshArray->Iterate();
	
	/*glUseProgram(m_programID);
	int view_proj_uniform = glGetUniformLocation(m_programID, "ProjectionView");
	unsigned int timeUniform = glGetUniformLocation(m_programID, "time");
	unsigned int heightScaleUniform = glGetUniformLocation(m_programID, "heightScale");

	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE,glm::value_ptr(m_camera->GetProjectionView()));
	glUniform1f(timeUniform, (float)glfwGetTime());
	float heightScale = 0.01f;
	glUniform1f(heightScaleUniform, heightScale);

	for (unsigned int i = 0; i < m_gl_info.size(); ++i)
	{
		glBindVertexArray(m_gl_info[i].m_VAO);
		glDrawElements(GL_TRIANGLES,m_gl_info[i].m_index_count, GL_UNSIGNED_INT, 0);
	}	*/

	glUseProgram(m_simpleObjProgramID);
	int view_proj_uniform = glGetUniformLocation(m_simpleObjProgramID, "ProjectionView");
//	unsigned int timeUniform = glGetUniformLocation(m_simpleObjProgramID, "time");
//	unsigned int heightScaleUniform = glGetUniformLocation(m_simpleObjProgramID, "heightScale");

	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE,glm::value_ptr(m_camera->GetProjectionView()));
//	glUniform1f(timeUniform, (float)glfwGetTime());
//	float heightScale = 0.01f;
//	glUniform1f(heightScaleUniform, heightScale);

	for (unsigned int i = 0; i < m_gl_info.size(); ++i)
	{
		glBindVertexArray(m_gl_info[i].m_VAO);
		glDrawElements(GL_TRIANGLES,m_gl_info[i].m_index_count, GL_UNSIGNED_INT, 0);
	}	
}

void Application2::GenerateGrid(unsigned int rows, unsigned int cols)
{
	Vertex* aoVertices = new Vertex[rows * cols];
	for(unsigned int r = 0; r < rows; ++r)
	{
		for(unsigned int c = 0; c < cols; ++c)
		{
			aoVertices[r * cols + c].position = vec4((float)c, 0, (float)r, 1);

			vec3 colour = vec3(sinf((c/(float)(cols - 1)) * (r/(float)(rows - 1))));
			aoVertices[r * cols + c].colour = vec4(colour, 1);
		}
	}

	unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) * 6];

	unsigned int index = 0;
	for(unsigned int r = 0; r < (rows - 1); ++r)
	{
		for(unsigned int c = 0; c < (cols - 1); ++c)
		{
			//triangle 1
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);

			//triangle 2
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
		}
	}

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4) * 2));
	//

	glBindVertexArray(0);

	delete[] aoVertices;
	delete[] auiIndices;

	m_meshArray->SetVAO(m_VAO);
	m_meshArray->SetIndexCount((rows - 1) * (cols - 1) * 6);
	m_meshArray->AddedMesh();
	m_meshArray->Iterate();
}

void Application2::GenerateCube()
{
	Vertex* aoVertices = new Vertex[8];

	aoVertices[0].position = vec4(-1, -1, -1, 1);
	aoVertices[1].position = vec4(-1, -1, 1, 1);
	aoVertices[2].position = vec4(1, -1, 1, 1);
	aoVertices[3].position = vec4(1, -1, -1, 1);
	aoVertices[4].position = vec4(-1, 1, -1, 1);
	aoVertices[5].position = vec4(-1, 1, 1, 1);
	aoVertices[6].position = vec4(1, 1, 1, 1);
	aoVertices[7].position = vec4(1, 1, -1, 1);

	for(int i = 0; i < 8; i++)
	{
		aoVertices[i].colour = vec4(vec3(1.0, 1.0, 1.0), 1);
	}

	unsigned int auiIndices[36] =
	{
		0, 7, 4,
		0, 3, 7,
		1, 4, 5,
		1, 0, 4,
		2, 5, 6,
		2, 1, 5,
		3, 6, 7,
		3, 2, 6,
		4, 7, 6,
		4, 6, 5,
		0, 2, 3,
		0, 1, 2
	};

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
		
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4) * 2));
	//

	glBindVertexArray(0);

	delete[] aoVertices;

	m_meshArray->SetVAO(m_VAO);
	m_meshArray->SetIndexCount(36);
	m_meshArray->AddedMesh();
	m_meshArray->Iterate();
}

void Application2::GenerateSimpleTexturePlane()
{

	float vertexData[] = {
		-5, 0, 5, 1, 0, 1,
		5, 0, 5, 1, 1, 1,
		5, 0, -5, 1, 1, 0,
		-5, 0, -5, 1, 0, 0
	};

	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_meshArray->SetVAO(m_VAO);
	m_meshArray->SetIndexCount(6);
	m_meshArray->AddedMesh();
	m_meshArray->Iterate();
}

void Application2::CreateSimpleShader()
{
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 Position; \
							layout(location=1) in vec4 Colour; \
							out vec4 vColour; \
							uniform mat4 ProjectionView; \
							uniform float time; \
							uniform float heightScale; \
							void main() { \
							vColour = Colour; \
							vec4 P = Position; \
							P.y += sin(time + Position.x * 32) * heightScale; \
							gl_Position = ProjectionView * P; }";

	const char* fsSource = "#version 410\n \
							in vec4 vColour; \
							out vec4 FragColor; \
							void main() { FragColor = vColour; }";

	//Compile shaders
	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	//
	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) 
	{
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n",infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	//


}

void Application2::CreateTextureShader()
{
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 Position; \
							layout(location=1) in vec2 TexCoord; \
							out vec2 vTexCoord; \
							uniform mat4 ProjectionView; \
							void main() { \
							vTexCoord = TexCoord; \
							gl_Position= ProjectionView * Position; }";

	const char* fsSource = "#version 410\n \
							in vec2 vTexCoord; \
							out vec4 FragColor; \
							uniform sampler2D diffuse; \
							void main() { \
							FragColor = texture(diffuse, vTexCoord); }";

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	//
	m_textureProgramID = glCreateProgram();
	glAttachShader(m_textureProgramID, vertexShader);
	glAttachShader(m_textureProgramID, fragmentShader);
	glLinkProgram(m_textureProgramID);

	glGetProgramiv(m_textureProgramID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) 
	{
		int infoLogLength = 0;
		glGetProgramiv(m_textureProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_textureProgramID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n",infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

void Application2::CreateSimpleObjShader()
{
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec3 Position; \
							layout(location=1) in vec3 Normal; \
							out vec4 vColour; \
							uniform mat4 ProjectionView; \
							void main() { \
							vColour = vec4(Normal, 1); \
							gl_Position = ProjectionView * vec4(Position, 1); }";

	const char* fsSource = "#version 410\n \
							in vec4 vColour; \
							out vec4 FragColor; \
							void main() { FragColor = vColour; }";

	//Compile shaders
	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	//
	m_simpleObjProgramID = glCreateProgram();
	glAttachShader(m_simpleObjProgramID, vertexShader);
	glAttachShader(m_simpleObjProgramID, fragmentShader);
	glLinkProgram(m_simpleObjProgramID);

	glGetProgramiv(m_simpleObjProgramID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) 
	{
		int infoLogLength = 0;
		glGetProgramiv(m_simpleObjProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_simpleObjProgramID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n",infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	//


}

void Application2::Load()
{
	m_camera = new FlyCamera(1.0f);
	m_camera->SetInputWindow(m_window);
	m_camera->SetUpPerspective(glm::pi<float>() * 0.25f, 16/9.f, 0.1f, 1000.f);

	CreateSimpleShader();
	CreateTextureShader();
	CreateSimpleObjShader();

	m_meshArray = new MeshArray();
	GenerateGrid(100, 100);
	GenerateCube();
	GenerateSimpleTexturePlane();
	//LoadObj();

	int imageWidth = 0, imageHeight = 0, imageFormat = 0;

	unsigned char* data = stbi_load("./data/textures/crate.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
	
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err = tinyobj::LoadObj(shapes, materials, "./data/models/testCube.obj");

	createOpenGLBuffers(shapes);
}

void Application2::createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
{
	m_gl_info.resize(shapes.size());

	for(unsigned int mesh_index = 0; mesh_index < shapes.size(); ++mesh_index)
	{
		glGenVertexArrays(1, &m_gl_info[mesh_index].m_VAO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_VBO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_IBO);
		glBindVertexArray(m_gl_info[mesh_index].m_VAO);

		unsigned int float_count = shapes[mesh_index].mesh.positions.size();
		float_count += shapes[mesh_index].mesh.normals.size();
		float_count += shapes[mesh_index].mesh.texcoords.size();

		std::vector<float> vertex_data;
		vertex_data.reserve(float_count);

		vertex_data.insert(vertex_data.end(),
			shapes[mesh_index].mesh.positions.begin(),
			shapes[mesh_index].mesh.positions.end());

		vertex_data.insert(vertex_data.end(),
			shapes[mesh_index].mesh.normals.begin(),
			shapes[mesh_index].mesh.normals.end());

		m_gl_info[mesh_index].m_index_count = shapes[mesh_index].mesh.indices.size();

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[mesh_index].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, 
			vertex_data.size() * sizeof(float), 
			vertex_data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[mesh_index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			shapes[mesh_index].mesh.indices.size() * sizeof(unsigned int), 
			shapes[mesh_index].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, 
			(void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	}
}
