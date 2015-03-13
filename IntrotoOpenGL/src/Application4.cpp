#include "Application4.h"

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "Gizmos.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

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

Application4::Application4()
{}

int Application4::Run()
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
		
		Gizmos::clear();

		Update(deltaTime);
		Draw();

		Gizmos::draw(m_camera->GetProjectionView());

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	delete m_meshArray;
	delete m_camera;

	glDeleteProgram(m_programID);
	glDeleteProgram(m_simpleObjProgramID);

	Gizmos::destroy();

	glfwDestroyWindow(m_window);
	glfwTerminate();
	return 0;
}

void Application4::Update(float deltaTime)
{
	/*animCountdown -= deltaTime;
	if(animCountdown < 0.0f)
	{
		anim += 1;
		if(anim == 4)
			anim = 0;

		animCountdown = 3.14f;
	}

	anim2 = anim + 1;
	if(anim2 == 4)
		anim2 = 0;

	float s = glm::cos(animCountdown) * 0.5f + 0.5f;

	glm::vec3 p = (1.0f - s) * m_positions[0 + anim] + s * m_positions[0 + anim2];

	glm::quat r = glm::slerp(m_rotations[0 + anim], m_rotations[0 + anim2], s);

	glm::mat4 m = glm::translate(p) * glm::toMat4(r);

	Gizmos::addTransform(m);
	Gizmos::addAABBFilled(p, glm::vec3(0.5f), glm::vec4(1, 0, 0, 1), &m);*/



	float s = glm::cos((float)glfwGetTime()) * 0.5f + 0.5f;

	glm::vec3 p = (1.0f - s) * m_hipFrames[0].position +
				s * m_hipFrames[1].position;
	
	glm::quat r = glm::slerp(m_hipFrames[0].rotation,
							m_hipFrames[1].rotation, s);

	m_hipBone = glm::translate(p) * glm::toMat4(r);

	p = (1.0f - s) * m_kneeFrames[0].position +
				s * m_kneeFrames[1].position;
	
	r = glm::slerp(m_kneeFrames[0].rotation,
							m_kneeFrames[1].rotation, s);

	m_kneeBone = m_hipBone * glm::translate(p) * glm::toMat4(r);

	p = (1.0f - s) * m_ankleFrames[0].position +
				s * m_ankleFrames[1].position;
	
	r = glm::slerp(m_ankleFrames[0].rotation,
							m_ankleFrames[1].rotation, s);

	m_ankleBone = m_kneeBone * glm::translate(p) * glm::toMat4(r);

	glm::vec3 hipPos = glm::vec3(m_hipBone[3].x, m_hipBone[3].y, m_hipBone[3].z);
	glm::vec3 kneePos = glm::vec3(m_kneeBone[3].x, m_kneeBone[3].y, m_kneeBone[3].z);
	glm::vec3 anklePos = glm::vec3(m_ankleBone[3].x, m_ankleBone[3].y, m_ankleBone[3].z);

	glm::vec3 half(0.5f);
	glm::vec4 pink(1, 0, 1, 1);

	Gizmos::addAABBFilled(hipPos, half, pink, &m_hipBone);
	Gizmos::addAABBFilled(kneePos, half, pink, &m_kneeBone);
	Gizmos::addAABBFilled(anklePos, half, pink, &m_ankleBone);

	m_camera->Update(deltaTime);
}

void Application4::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	////Hardcoded test values
	vec3 lightDirection = vec3(-0.5f, 0.5f/*glfwGetTime() * 0.1f*/, 0.5f);
	vec3 lightColour = vec3(1.0f, 0.7f, 0.0f);
	float specularPower = 1.0f;

	glUseProgram(m_simpleObjProgramID);
	unsigned int viewProjectionUniform	= glGetUniformLocation(m_simpleObjProgramID, "ProjectionView");
	unsigned int lightDirectionUniform	= glGetUniformLocation(m_simpleObjProgramID, "LightDirection");
	unsigned int lightColourUniform		= glGetUniformLocation(m_simpleObjProgramID, "LightColour");
	unsigned int cameraPositionUniform	= glGetUniformLocation(m_simpleObjProgramID, "CameraPosition");
	unsigned int specularPowerUniform	= glGetUniformLocation(m_simpleObjProgramID, "SpecularPower");
		
	glUniformMatrix4fv(viewProjectionUniform, 1, GL_FALSE,glm::value_ptr(m_camera->GetProjectionView()));
	glUniform3fv(lightDirectionUniform, 1, glm::value_ptr(lightDirection));	
	glUniform3fv(lightColourUniform, 1, glm::value_ptr(lightColour));
	glUniform3fv(cameraPositionUniform, 1, glm::value_ptr(m_camera->GetPosition()));
	glUniform1f(specularPowerUniform, specularPower);
	
	for (unsigned int i = 0; i < m_gl_info.size(); ++i)
	{
		glBindVertexArray(m_gl_info[i].m_VAO);
		glDrawElements(GL_TRIANGLES,m_gl_info[i].m_index_count, GL_UNSIGNED_INT, 0);
	}	


	//Texture plane draw
	glUseProgram(m_textureProgramID);

	//bind camera
	int loc = glGetUniformLocation(m_textureProgramID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_camera->GetProjectionView()[0][0]));

	//set texture slots
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalMap);

	//tell shader where it is
	loc = glGetUniformLocation(m_textureProgramID, "diffuse");
	glUniform1i(loc, 0);
	loc = glGetUniformLocation(m_textureProgramID, "normal");
	glUniform1i(loc, 1);

	//bind light
	vec3 light(sin(glfwGetTime()), 1.0f, cos(glfwGetTime()));
	loc = glGetUniformLocation(m_textureProgramID, "LightDirection");
	glUniform3f(loc, light.x, light.y, light.z);

	//draw
	glBindVertexArray(m_VAOtest);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Application4::GenerateSimpleTexturePlane()
{

	VertexAdv vertexData[] = {
		{-5, 0, 5, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1},
		{5, 0, 5, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1},
		{5, 0, -5, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
		{-5, 0, -5, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0}
	};

	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAdv) * 4, vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexAdv), ((char*)0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAdv), ((char*)0) + 48);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexAdv), ((char*)0) + 16);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(VertexAdv), ((char*)0) + 32);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//test
	m_VAOtest = m_VAO;

	m_meshArray->SetVAO(m_VAO);
	m_meshArray->SetIndexCount(6);
	m_meshArray->AddedMesh();
	m_meshArray->Iterate();

	

	//5721
}

void Application4::CreateSimpleShader()
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

//Updating texture shader
void Application4::CreateTextureShader()
{
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 Position; \
							layout(location=1) in vec2 TexCoord; \
							layout(location=2) in vec4 Normal; \
							layout(location=3) in vec4 Tangent; \
							out vec2 vTexCoord; \
							out vec3 vNormal; \
							out vec3 vTangent; \
							out vec3 vBiTangent; \
							uniform mat4 ProjectionView; \
							void main() { \
							vTexCoord = TexCoord; \
							vNormal = Normal.xyz; \
							vTangent = Tangent.xyz; \
							vBiTangent = cross(vNormal, vTangent); \
							gl_Position= ProjectionView * Position; }";

	const char* fsSource = "#version 410\n \
							in vec2 vTexCoord; \
							in vec3 vNormal; \
							in vec3 vTangent; \
							in vec3 vBiTangent; \
							out vec4 FragColor; \
							uniform vec3 LightDirection; \
							uniform sampler2D diffuse; \
							uniform sampler2D normal; \
							void main() { \
							mat3 TBN = mat3(normalize(vTangent), normalize(vBiTangent), normalize(vNormal)); \
							vec3 N = texture(normal, vTexCoord).xyz * 2 - 1; \
							float d = max(0, dot(normalize(TBN * N), normalize(LightDirection))); \
							FragColor = texture(diffuse, vTexCoord); \
							FragColor.rgb = FragColor.rgb * d;}";

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

void Application4::CreateSimpleObjShader()
{
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec3 Position; \
							layout(location=1) in vec3 Normal; \
							out vec4 vNormal; \
							out vec4 vPosition; \
							uniform mat4 ProjectionView; \
							void main() { \
							vNormal = vec4(Normal, 1); \
							gl_Position = ProjectionView * vec4(Position, 1); }";

	const char* fsSource = "#version 410\n \
							in vec4 vNormal; \
							in vec4 vPosition; \
							out vec4 FragColor; \
							uniform vec3 LightDirection; \
							uniform vec3 LightColour; \
							uniform vec3 CameraPosition; \
							uniform float SpecularPower; \
							void main() { \
							float d = max(0, dot(normalize(vNormal.xyz), LightDirection)); \
							vec3 E = normalize(CameraPosition - vPosition.xyz); \
							vec3 R = reflect(-LightDirection, vNormal.xyz); \
							float s = max(0, dot(E, R)); \
							s = pow( s, SpecularPower); \
							FragColor = vec4(LightColour * d + LightColour * s ,1); }";

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

void Application4::Load()
{
	m_camera = new FlyCamera(1.0f);
	m_camera->SetInputWindow(m_window);
	m_camera->SetUpPerspective(glm::pi<float>() * 0.25f, 16/9.f, 0.1f, 1000.f);

	CreateSimpleShader();
	CreateTextureShader();
	CreateSimpleObjShader();

	m_meshArray = new MeshArray();
	GenerateSimpleTexturePlane();

	int imageWidth = 0, imageHeight = 0, imageFormat = 0;

	//load diffuse map
	unsigned char* data = stbi_load("./data/textures/rock_diffuse.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB /*REMEMBER RGBA*/, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	//load normal map
	data = stbi_load("./data/textures/rock_normal.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_normalMap);
	glBindTexture(GL_TEXTURE_2D, m_normalMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
		
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err = tinyobj::LoadObj(shapes, materials, "./data/models/vanquish.obj");

	createOpenGLBuffers(shapes);

	m_positions[0] = glm::vec3(10, 5, 10);
	m_positions[1] = glm::vec3(-10, 0, -10);
	m_positions[2] = glm::vec3(-10, 10, -10);
	m_positions[3] = glm::vec3(0, 0, 40);
	m_rotations[0] = glm::quat(glm::vec3(0, -1, 0));
	m_rotations[1] = glm::quat(glm::vec3(0, 1, 0));
	m_rotations[2] = glm::quat(glm::vec3(0, 0, 1));
	m_rotations[3] = glm::quat(glm::vec3(1, 0, 0));
	anim = 0;
	anim2 = 1;
	animCountdown = 0.0f;

	m_hipFrames[0].position = glm::vec3(0, 5, 0);
	m_hipFrames[0].rotation = glm::quat(glm::vec3(1, 0, 0));
	m_hipFrames[1].position = glm::vec3(0, 5, 0);
	m_hipFrames[1].rotation = glm::quat(glm::vec3(-1, 0, 0));

	m_kneeFrames[0].position = glm::vec3(0, -2.5f, 0);
	m_kneeFrames[0].rotation = glm::quat(glm::vec3(1, 0, 0));
	m_kneeFrames[1].position = glm::vec3(0, -2.5f, 0);
	m_kneeFrames[1].rotation = glm::quat(glm::vec3(0, 0, 0));

	m_ankleFrames[0].position = glm::vec3(0, -2.5f, 0);
	m_ankleFrames[0].rotation = glm::quat(glm::vec3(-1, 0, 0));
	m_ankleFrames[1].position = glm::vec3(0, -2.5f, 0);
	m_ankleFrames[1].rotation = glm::quat(glm::vec3(0, 0, 0));
}

void Application4::createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
{
	m_gl_info.resize(shapes.size());

	for(unsigned int mesh_index = 0; mesh_index < shapes.size(); ++mesh_index)
	{
		glGenVertexArrays(1, &m_gl_info[mesh_index].m_VAO);
		glBindVertexArray(m_gl_info[mesh_index].m_VAO);
		
		glGenBuffers(1, &m_gl_info[mesh_index].m_VBO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_IBO);		

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

void Application4::cleanupOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
{
	m_gl_info.resize(shapes.size());

	for(unsigned int mesh_index = 0; mesh_index < shapes.size(); ++mesh_index)
	{
		glDeleteVertexArrays(1, &m_gl_info[mesh_index].m_VAO);
		glDeleteBuffers(1, &m_gl_info[mesh_index].m_VBO);
		glDeleteBuffers(1, &m_gl_info[mesh_index].m_IBO);
	}
}