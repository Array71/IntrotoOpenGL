#include "Renderer.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

Renderer::Renderer(FlyCamera* camera)
{
	m_camera = camera;
}

Renderer::~Renderer()
{
	glDeleteProgram(m_programObjID);
	glDeleteProgram(m_programTexturePlaneID);

	CleanupOpenGLBuffers(m_FBX);
}

void Renderer::Update(float deltaTime)
{
	float s = glm::cos((float)glfwGetTime()) * 0.5f + 0.5f;


	//All stuff for test skeleton
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

	//SKELETON STUFF
	FBXSkeleton* skeleton = m_FBX->getSkeletonByIndex(0);
	FBXAnimation* animation = m_FBX->getAnimationByIndex(0);

	skeleton->evaluate(animation, glfwGetTime());

	for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
	{
		skeleton->m_nodes[bone_index]->updateGlobalTransform();
	}

	//m_particleEmitter->Update(deltaTime, m_camera->GetTransform());
	m_gpuParticleEmitter->Update(deltaTime);
}

void Renderer::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Hardcoded test values
	vec3 lightDirection = vec3(-0.5f, 0.5f/*glfwGetTime() * 0.1f*/, 0.5f);
	vec3 lightColour = vec3(1.0f, 0.7f, 0.0f);
	float specularPower = 1.0f;
	FBXSkeleton* skeleton = m_FBX->getSkeletonByIndex(0);
	skeleton->updateBones();

	glUseProgram(m_programObjID);
	unsigned int viewProjectionUniform = glGetUniformLocation(m_programObjID, "ProjectionView");
	unsigned int lightDirectionUniform = glGetUniformLocation(m_programObjID, "LightDirection");
	unsigned int lightColourUniform = glGetUniformLocation(m_programObjID, "LightColour");
	unsigned int cameraPositionUniform = glGetUniformLocation(m_programObjID, "CameraPosition");
	unsigned int specularPowerUniform = glGetUniformLocation(m_programObjID, "SpecularPower");
	unsigned int bonesUniform = glGetUniformLocation(m_programObjID, "Bones");

	glUniformMatrix4fv(viewProjectionUniform, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjectionView()));
	glUniform3fv(lightDirectionUniform, 1, glm::value_ptr(lightDirection));
	glUniform3fv(lightColourUniform, 1, glm::value_ptr(lightColour));
	glUniform3fv(cameraPositionUniform, 1, glm::value_ptr(m_camera->GetPosition()));
	glUniform1f(specularPowerUniform, specularPower);
	glUniformMatrix4fv(bonesUniform, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	int iDiffuseLoc = glGetUniformLocation(m_programObjID, "Diffuse");
	glUniform1i(iDiffuseLoc, 0);

	for (unsigned int i = 0; i < m_FBX->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_FBX->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	//Texture plane draw
	glUseProgram(m_programTexturePlaneID);

	//bind camera
	int loc = glGetUniformLocation(m_programTexturePlaneID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_camera->GetProjectionView()[0][0]));

	//set texture slots
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalMap);

	//tell shader where it is
	loc = glGetUniformLocation(m_programTexturePlaneID, "diffuse");
	glUniform1i(loc, 0);
	loc = glGetUniformLocation(m_programTexturePlaneID, "normal");
	glUniform1i(loc, 1);

	//bind light
	vec3 light(sin(glfwGetTime()), 1.0f, cos(glfwGetTime()));
	loc = glGetUniformLocation(m_programTexturePlaneID, "LightDirection");
	glUniform3f(loc, light.x, light.y, light.z);

	//draw
	glBindVertexArray(m_VAOtest);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	/*glUseProgram(m_particleProgramID);
	loc = glGetUniformLocation(m_particleProgramID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &m_camera->GetProjectionView()[0][0]);*/
	//m_particleEmitter->Draw();
	m_gpuParticleEmitter->Draw((float)glfwGetTime(), m_camera->GetTransform(), m_camera->GetProjectionView());
}

void Renderer::Load()
{
	CreateShader(m_programObjID, "./data/shaders/obj.vert", "./data/shaders/obj.frag");
	CreateShader(m_programTexturePlaneID, "./data/shaders/texPlane.vert", "./data/shaders/texPlane.frag");
	CreateShader(m_programTexturePlaneSimpleID, "./data/shaders/texPlaneSimple.vert", "./data/shaders/texPlaneSimple.frag");

	//m_meshArray = new MeshArray();
	GenerateTexturePlane();

	m_gpuParticleEmitter = new GPUParticleEmitter();
	m_gpuParticleEmitter->Initialise(100000, 0.1f, 5.0f, 5, 20, 1, 0.1f, glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1));

	m_FBX = new FBXFile();
	m_FBX->load("data/characters/Pyro/pyro.fbx");
	m_FBX->initialiseOpenGLTextures();
	CreateOpenGLBuffers(m_FBX);

	//Textured plane
	//----------
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
	//----------
	
	//Test content for gizmos animation
	//----------
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
	//----------
}

unsigned int Renderer::LoadShader(unsigned int type, const char* path)
{
	FILE* file = fopen(path, "rb");
	if (file == nullptr)
		return 0;

	//read shader source
	fseek(file, 0, SEEK_END);
	unsigned int length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* source = new char[length + 1];
	memset(source, 0, length + 1);
	fread(source, sizeof(char), length, file);
	fclose(file);

	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, 0);
	glCompileShader(shader);
	delete[] source;

	return shader;
}

void Renderer::CreateShader(unsigned int &shader, const char* vert, const char* frag)
{
	unsigned int vs = LoadShader(GL_VERTEX_SHADER, vert);
	unsigned int fs = LoadShader(GL_FRAGMENT_SHADER, frag);

	int success = GL_FALSE;
	//
	//unsigned int shader;

	shader = glCreateProgram();
	glAttachShader(shader, vs);
	glAttachShader(shader, fs);
	glLinkProgram(shader);

	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(shader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fs);
	glDeleteShader(vs);
}

void Renderer::CreateOpenGLBuffers(FBXFile* fbx)
{
	int numMeshes = fbx->getMeshCount();

	for (int i = 0; i < numMeshes; i++)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);

		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		glBufferData(GL_ARRAY_BUFFER,
			mesh->m_vertices.size() * sizeof(FBXVertex),
			mesh->m_vertices.data(), GL_STATIC_DRAW);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->m_indices.size() * sizeof(unsigned int),
			mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normal
		glEnableVertexAttribArray(2); //texture coordinates
		glEnableVertexAttribArray(3); //tangents
		glEnableVertexAttribArray(4); //weights
		glEnableVertexAttribArray(5); //indices

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE,
			sizeof(FBXVertex),
			((char*)0) + FBXVertex::NormalOffset);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TangentOffset);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}

void Renderer::CleanupOpenGLBuffers(FBXFile* fbx)
{
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}

void Renderer::GenerateTexturePlane()
{

	VertexAdv vertexData[] = {
		{ -5, 0, 5, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1 },
		{ 5, 0, 5, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1 },
		{ 5, 0, -5, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
		{ -5, 0, -5, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 }
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
	
	/*	m_meshArray->SetVAO(m_VAO);
	m_meshArray->SetIndexCount(6);
	m_meshArray->AddedMesh();
	m_meshArray->Iterate();*/



	//5721
}