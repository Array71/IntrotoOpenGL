#ifndef APPLICATION5_H
#define APPLICATION5_H

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "MeshArray.h"

#include "Particles.h"

//#include "tiny_obj_loader.h"
#include "FBXFile.h"

class MeshArray;
class FlyCamera;
struct OpenGLInfo;
struct KeyFrame;

class Application5
{
public:
	Application5();
	~Application5();

	int Run();

	unsigned int LoadShader(unsigned int type, const char* path);

	void CreateSimpleShader();
	void CreateTextureShader();
	void CreateSimpleObjShader();
	void CreateParticleShader();

	void Load();

	void Update(float deltaTime);
	void Draw();

private:
	//	void GenerateGrid(unsigned int rows, unsigned int cols);
	//	void GenerateCube();
	void GenerateSimpleTexturePlane();

	void createOpenGLBuffers(FBXFile* fbx);
	void cleanupOpenGLBuffers(FBXFile* fbx);

private:
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_VAOtest;

	unsigned int m_programID; //unused
	unsigned int m_textureProgramID;
	unsigned int m_simpleObjProgramID;
	unsigned int m_particleProgramID;

	unsigned int m_texture, m_normalMap;

	//unsigned int m_texture;

	ParticleEmitter* m_particleEmitter;
	GPUParticleEmitter* m_gpuParticleEmitter;

	FBXFile* m_FBX;

	MeshArray* m_meshArray;
	FlyCamera* m_camera;
	GLFWwindow* m_window;

	std::vector<OpenGLInfo> m_gl_info;

	glm::vec3 m_positions[4];
	glm::quat m_rotations[4];

	unsigned int anim;
	unsigned int anim2;
	float animCountdown;

	struct KeyFrame
	{
		glm::vec3 position;
		glm::quat rotation;
	};

	KeyFrame m_hipFrames[2];
	KeyFrame m_kneeFrames[2];
	KeyFrame m_ankleFrames[2];

	glm::mat4 m_hipBone;
	glm::mat4 m_kneeBone;
	glm::mat4 m_ankleBone;
};




#endif